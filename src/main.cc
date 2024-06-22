#include <iostream>
#include <limits>
#include <memory>
#include <optional>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "hittable_list.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

Color RayColor(const Ray& ray, const Hittable& world) {
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

int main() {
  // Image.
  const Float aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;

  const int real_height = static_cast<int>(image_width / aspect_ratio);
  const int image_height = (real_height < 1) ? 1 : real_height;

  // World.
  HittableList world;
  world.Add(std::make_shared<Sphere>(Point3{0, 0, -1}, 0.5));
  world.Add(std::make_shared<Sphere>(Point3{0.5, 0.5, -1}, 0.25));
  world.Add(std::make_shared<Sphere>(Point3{-0.5, 0.5, -1}, 0.25));
  world.Add(std::make_shared<Sphere>(Point3{0, -100.5, -1}, 100));

  // Camera.
  const Float focal_length = 1.0;
  const Float viewport_height = 2.0;
  const Float viewport_width =
      viewport_height * (static_cast<Float>(image_width) / image_height);
  const Point3 camera_center{0, 0, 0};

  const Vec3 viewport_u = Vec3(viewport_width, 0, 0);
  const Vec3 viewport_v = Vec3(0, -viewport_height, 0);

  const Vec3 pixel_delta_u = viewport_u / image_width;
  const Vec3 pixel_delta_v = viewport_v / image_height;

  const Vec3 viewport_upper_left = camera_center - Vec3(0, 0, focal_length) -
                                   viewport_u / 2 - viewport_v / 2;

  const Vec3 origin_pixel_location =
      viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int v = 0; v < image_height; v++) {
    std::clog << "\rScanlines remaining: " << (image_height - v) << ' '
              << std::flush;

    for (int u = 0; u < image_width; u++) {
      auto pixel_center =
          origin_pixel_location + (u * pixel_delta_u) + (v * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      Ray ray(camera_center, ray_direction);

      auto pixel_color = RayColor(ray, world);
      WriteColor(std::cout, pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";
}
