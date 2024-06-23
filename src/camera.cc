#include "camera.h"

#include <iostream>
#include <limits>
#include <optional>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "utils.h"
#include "vec3.h"

void Camera::Render(const Hittable& world) {
  Initialize();

  std::cout << "P3\n" << image_width_ << ' ' << image_height_ << "\n255\n";

  for (int j = 0; j < image_height_; j++) {
    std::clog << "\rScanlines remaining: " << (image_height_ - j) << ' '
              << std::flush;

    for (int i = 0; i < image_width_; i++) {
      Color pixel_color{};
      for (int sample = 0; sample < samples_per_pixel_; sample++) {
        const Ray ray = GetRay(i, j);
        pixel_color += RayColor(ray, world);
      }

      WriteColor(std::cout, pixel_samples_scale_ * pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";
}

void Camera::Initialize() {
  pixel_samples_scale_ = 1.0 / samples_per_pixel_;

  center_ = Point3{};

  const Float focal_length = 1.0;
  const Float viewport_height = 2.0;
  const Float viewport_width =
      viewport_height * (static_cast<Float>(image_width_) / image_height_);

  const Vec3 viewport_u{viewport_width, 0, 0};
  const Vec3 viewport_v{0, -viewport_height, 0};

  pixel_delta_u_ = viewport_u / image_width_;
  pixel_delta_v_ = viewport_v / image_height_;

  const Vec3 viewport_upper_left =
      center_ - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
  upper_left_pixel_location_ =
      viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
}

Ray Camera::GetRay(int i, int j) const {
  const Vec3 offset{static_cast<Float>(RandomDouble() - 0.5),
                    static_cast<Float>(RandomDouble() - 0.5), 0};
  const Vec3 pixel_sample = upper_left_pixel_location_ +
                            ((i + offset.x()) * pixel_delta_u_) +
                            ((j + offset.y()) * pixel_delta_v_);

  const Vec3 ray_direction = pixel_sample - center_;
  return Ray{center_, ray_direction};
}

Color Camera::RayColor(const Ray& ray, const Hittable& world) const {
  const Color white{1.0, 1.0, 1.0};
  const Color blue{0.5, 0.7, 1.0};

  const Float min = 0;
  const Float max = std::numeric_limits<Float>::infinity();
  std::optional<HitRecord> record = world.Hit(ray, min, max);
  if (record.has_value()) {
    return 0.5 * (record->normal() + white);
  }

  const Vec3 unit_direction = UnitVector(ray.direction());
  const Float a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * white + a * blue;
}