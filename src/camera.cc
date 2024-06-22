#include "camera.h"

#include <iostream>
#include <limits>
#include <optional>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

void Camera::Render(const Hittable& world) {
  Initialize();

  std::cout << "P3\n" << image_width_ << ' ' << image_height_ << "\n255\n";

  for (int j = 0; j < image_height_; j++) {
    std::clog << "\rScanlines remaining: " << (image_height_ - j) << ' '
              << std::flush;

    for (int i = 0; i < image_width_; i++) {
      const Vec3 pixel_center = upper_left_pixel_location_ +
                                (i * pixel_delta_u_) + (j * pixel_delta_v_);
      const Vec3 ray_direction = pixel_center - center_;
      Ray ray{center_, ray_direction};

      Color pixel_color = RayColor(ray, world);
      WriteColor(std::cout, pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";
}

void Camera::Initialize() {
  const int real_height = static_cast<int>(image_width_ / aspect_ratio_);
  image_height_ = (real_height < 1) ? 1 : real_height;

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