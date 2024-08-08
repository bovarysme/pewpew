#include "camera.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <stop_token>

#include "app_settings.h"
#include "color.h"
#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"
#include "vec3.h"

void Camera::Initialize(SettingsUpdateType type) {
  const int data_size =
      settings_.image_width * settings_.image_height * num_color_components_;
  pixel_data_ = std::vector<Float>(data_size, 0.0);
  if (type == SettingsUpdateType::kUpdateTextureAndSettings) {
    const std::lock_guard<std::mutex> guard(image_data_mutex_);
    image_data_ = std::vector<uint8_t>(data_size, 0);
  }

  is_rendering_ = false;
  done_rendering_ = false;

  current_phase_ = 0;
  last_phase_ = settings_.samples_per_pixel_log2 + 1;
  current_phase_samples_per_pixel_ = 0;
  accumulated_samples_per_pixel_ = 0;
  pixel_samples_scale_ = 0;

  global_render_time_ = 0.0;
  phase_render_time_ = 0.0;
  scanlines_rendered_ = 0;

  center_ = settings_.look_from;

  const Float theta = DegreesToRadians(settings_.fov);
  const Float h = std::tan(theta / 2);
  const Float viewport_height = 2.0 * h * settings_.focus_distance;
  const Float viewport_width =
      viewport_height *
      (static_cast<Float>(settings_.image_width) / settings_.image_height);

  w_ = UnitVector(settings_.look_from - settings_.look_at);
  u_ = UnitVector(Cross(settings_.view_up, w_));
  v_ = Cross(w_, u_);

  const Vec3 viewport_u = viewport_width * u_;
  const Vec3 viewport_v = viewport_height * -v_;

  pixel_delta_u_ = viewport_u / settings_.image_width;
  pixel_delta_v_ = viewport_v / settings_.image_height;

  const Vec3 viewport_upper_left = center_ - (settings_.focus_distance * w_) -
                                   viewport_u / 2 - viewport_v / 2;
  upper_left_pixel_location_ =
      viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);

  const Float defocus_radius =
      settings_.focus_distance *
      tan(DegreesToRadians(settings_.defocus_angle / 2));
  defocus_disk_u_ = u_ * defocus_radius;
  defocus_disk_v_ = v_ * defocus_radius;
}

void Camera::InitializePhase() {
  is_rendering_ = true;

  current_phase_++;
  // Accumulated samples per pixel should double for each phase:
  // 1 + 1 + 2 + 4...
  current_phase_samples_per_pixel_ =
      current_phase_ == 1 ? 1 : 1 << (current_phase_ - 2);
  accumulated_samples_per_pixel_ += current_phase_samples_per_pixel_;
  pixel_samples_scale_ = 1.0 / accumulated_samples_per_pixel_;

  scanlines_rendered_ = 0;
}

void Camera::Render(std::stop_token token, const Hittable& world) {
  std::chrono::time_point phase_start_time = std::chrono::system_clock::now();

  // clang-format off
  #pragma omp parallel for num_threads(6)
  // clang-format on
  for (int j = 0; j < settings_.image_height; j++) {
    // Prevent render invalidation during the first phase (1 sample per pixel).
    // This increases the frequency of image updates when changing app settings.
    if (token.stop_requested() && current_phase_ > 1) {
      continue;
    }

    scanlines_rendered_++;

    for (int i = 0; i < settings_.image_width; i++) {
      Color pixel_color{};
      for (int sample = 0; sample < current_phase_samples_per_pixel_;
           sample++) {
        const Ray ray = GetRay(i, j);
        pixel_color += RayColor(ray, settings_.max_depth, world);
      }

      const int index = (j * settings_.image_width + i) * num_color_components_;
      pixel_data_[index] += pixel_color.x();
      pixel_data_[index + 1] += pixel_color.y();
      pixel_data_[index + 2] += pixel_color.z();
    }
  }

  bool is_render_invalidated = token.stop_requested() && current_phase_ > 1;
  if (!is_render_invalidated) {
    StoreImage();
  }

  std::chrono::time_point phase_end_time = std::chrono::system_clock::now();
  phase_render_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                           phase_end_time - phase_start_time)
                           .count();
  global_render_time_ += phase_render_time_;

  if (current_phase_ >= last_phase_) {
    done_rendering_ = true;
  }

  // This must be unset last to prevent a race condition where the rendering
  // state would be updated before `done_rendering_` could be set.
  is_rendering_ = false;
}

void Camera::StoreImage() {
  const std::lock_guard<std::mutex> guard(image_data_mutex_);

  for (int j = 0; j < settings_.image_height; j++) {
    for (int i = 0; i < settings_.image_width; i++) {
      for (int k = 0; k < num_color_components_; k++) {
        const int index =
            (j * settings_.image_width + i) * num_color_components_ + k;
        image_data_[index] =
            TransformColor(pixel_data_[index] * pixel_samples_scale_);
      }
    }
  }
}

Float Camera::Progress() const {
  return scanlines_rendered_ / static_cast<Float>(settings_.image_height - 1);
}

void Camera::CopyTo(int* buffer) {
  const std::lock_guard<std::mutex> guard(image_data_mutex_);

  for (int j = 0; j < settings_.image_height; j++) {
    for (int i = 0; i < settings_.image_width; i++) {
      int src_index = (j * settings_.image_width + i) * num_color_components_;
      uint8_t r = image_data_[src_index];
      uint8_t g = image_data_[src_index + 1];
      uint8_t b = image_data_[src_index + 2];

      int dest_index = j * settings_.image_width + i;
      buffer[dest_index] = (r << 16) | (g << 8) | b;
    }
  }
}

Ray Camera::GetRay(int i, int j) const {
  const Vec3 offset{static_cast<Float>(RandomFloat() - 0.5),
                    static_cast<Float>(RandomFloat() - 0.5), 0};
  const Vec3 pixel_sample = upper_left_pixel_location_ +
                            ((i + offset.x()) * pixel_delta_u_) +
                            ((j + offset.y()) * pixel_delta_v_);

  const Point3 ray_origin =
      (settings_.defocus_angle <= 0) ? center_ : SampleDefocusDisk();
  const Vec3 ray_direction = pixel_sample - ray_origin;
  return Ray{ray_origin, ray_direction};
}

Color Camera::RayColor(const Ray& ray, int depth, const Hittable& world) const {
  const Color black{0.0, 0.0, 0.0};
  if (depth <= 0) {
    return black;
  }

  const Float min = 0.001;
  const Float max = std::numeric_limits<Float>::infinity();
  std::optional<HitRecord> hit_record = world.Hit(ray, min, max);
  if (hit_record.has_value()) {
    if (hit_record->material() == nullptr) {
      return black;
    }

    Material* material = hit_record->material();
    std::optional<ScatterRecord> scatter_record =
        material->Scatter(ray, hit_record.value());
    if (!scatter_record.has_value()) {
      return black;
    }

    return scatter_record->attenuation() *
           RayColor(scatter_record->scattered(), depth - 1, world);
  }

  const Color white{1.0, 1.0, 1.0};
  const Color blue{0.5, 0.7, 1.0};

  const Vec3 unit_direction = UnitVector(ray.direction());
  const Float a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * white + a * blue;
}

void Camera::WriteImage() {
  const std::lock_guard<std::mutex> guard(image_data_mutex_);

  std::cout << "P3\n"
            << settings_.image_width << ' ' << settings_.image_height
            << "\n255\n";

  for (int j = 0; j < settings_.image_height; j++) {
    for (int i = 0; i < settings_.image_width; i++) {
      const int index = (j * settings_.image_width + i) * num_color_components_;
      std::cout << image_data_[index] << ' ' << image_data_[index + 1] << ' '
                << image_data_[index + 2] << '\n';
    }
  }
}

Point3 Camera::SampleDefocusDisk() const {
  Point3 p = RandomInUnitDisk();
  return center_ + (p.x() * defocus_disk_u_) + (p.y() * defocus_disk_v_);
}
