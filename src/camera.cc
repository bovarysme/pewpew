#include "camera.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <stop_token>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"
#include "vec3.h"

void Camera::Initialize() {
  {
    const std::lock_guard<std::mutex> guard(pixel_data_mutex_);
    pixel_data_.reserve(settings_.image_width * settings_.image_height *
                        num_color_components_);
  }

  pixel_samples_scale_ = 1.0 / settings_.samples_per_pixel;

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

  Float defocus_radius = settings_.focus_distance *
                         tan(DegreesToRadians(settings_.defocus_angle / 2));
  defocus_disk_u_ = u_ * defocus_radius;
  defocus_disk_v_ = v_ * defocus_radius;
}

void Camera::Render(std::stop_token token, const Hittable& world) {
  for (int j = 0; j < settings_.image_height && !token.stop_requested(); j++) {
    progress_ = j / static_cast<Float>(settings_.image_height - 1);

    for (int i = 0; i < settings_.image_width; i++) {
      Color pixel_color{};
      for (int sample = 0; sample < settings_.samples_per_pixel; sample++) {
        const Ray ray = GetRay(i, j);
        pixel_color += RayColor(ray, settings_.max_depth, world);
      }

      const int index = (j * settings_.image_width + i) * num_color_components_;
      {
        const std::lock_guard<std::mutex> guard(pixel_data_mutex_);
        StoreColor(pixel_data_, index, pixel_samples_scale_ * pixel_color);
      }
    }
  }

  // std::clog << "\rWriting image.        " << std::flush;
  // WriteImage();
  // std::clog << "\rDone.                 " << std::flush;

  is_rendering_ = false;
}

void Camera::CopyTo(int* buffer) {
  const std::lock_guard<std::mutex> guard(pixel_data_mutex_);

  for (int j = 0; j < settings_.image_height; j++) {
    for (int i = 0; i < settings_.image_width; i++) {
      int src_index = (j * settings_.image_width + i) * num_color_components_;
      uint8_t r = static_cast<uint8_t>(pixel_data_[src_index]);
      uint8_t g = static_cast<uint8_t>(pixel_data_[src_index + 1]);
      uint8_t b = static_cast<uint8_t>(pixel_data_[src_index + 2]);

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
  const std::lock_guard<std::mutex> guard(pixel_data_mutex_);

  std::cout << "P3\n"
            << settings_.image_width << ' ' << settings_.image_height
            << "\n255\n";

  for (int j = 0; j < settings_.image_height; j++) {
    for (int i = 0; i < settings_.image_width; i++) {
      const int index = (j * settings_.image_width + i) * num_color_components_;
      std::cout << pixel_data_[index] << ' ' << pixel_data_[index + 1] << ' '
                << pixel_data_[index + 2] << '\n';
    }
  }
}

Point3 Camera::SampleDefocusDisk() const {
  Point3 p = RandomInUnitDisk();
  return center_ + (p.x() * defocus_disk_u_) + (p.y() * defocus_disk_v_);
}
