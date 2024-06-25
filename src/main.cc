#include <memory>

#include "camera.h"
#include "color.h"
#include "dielectric.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "sphere.h"
#include "vec3.h"

int main() {
  HittableList world;
  Lambertian ground_material{Color{0.5, 0.5, 0.5}};
  world.Add(
      std::make_shared<Sphere>(Point3{0, -1000, 0}, 1000, &ground_material));

  std::vector<std::unique_ptr<Material>> materials;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      Float choose_mat = RandomFloat();
      Float center_x = a + 0.9 * RandomFloat();
      Float center_z = b + 0.9 * RandomFloat();
      Point3 center{center_x, 0.2, center_z};

      if ((center - Point3{4, 0.2, 0}).length() > 0.9) {
        if (choose_mat < 0.8) {
          // Diffuse.
          Color albedo = Color::Random() * Color::Random();
          materials.push_back(std::make_unique<Lambertian>(albedo));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        } else if (choose_mat < 0.95) {
          // Metal.
          Color albedo = Color::Random(0.5, 1);
          Float fuzz = RandomFloat(0, 0.5);
          materials.push_back(std::make_unique<Metal>(albedo, fuzz));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        } else {
          // Glass.
          materials.push_back(std::make_unique<Dielectric>(1.5));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        }
      }
    }
  }

  Dielectric material1{1.5};
  world.Add(std::make_shared<Sphere>(Point3{0, 1, 0}, 1.0, &material1));

  Lambertian material2{Color{0.4, 0.2, 0.1}};
  world.Add(std::make_shared<Sphere>(Point3{-4, 1, 0}, 1.0, &material2));

  Metal material3{Color{0.7, 0.6, 0.5}, 0.0};
  world.Add(std::make_shared<Sphere>(Point3{4, 1, 0}, 1.0, &material3));

  CameraSettings settings{
      .image_width = 1280,
      .image_height = 720,
      .samples_per_pixel = 128,
      .max_depth = 16,

      .fov = 20,
      .look_from = Point3{13, 2, 3},
      .look_at = Point3{0, 0, 0},
      .view_up = Vec3{0, 1, 0},

      .defocus_angle = 0.6,
      .focus_distance = 10.0,
  };

  Camera camera{settings};
  camera.Render(world);
}