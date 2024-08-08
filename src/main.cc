#include <SDL2/SDL.h>

#include <memory>

#include "app.h"
#include "camera.h"
#include "color.h"
#include "dielectric.h"
#include "hittable.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "sphere.h"
#include "vec3.h"

int main(int argc, char** argv) {
  std::vector<std::unique_ptr<Hittable>> geometries;
  std::vector<std::unique_ptr<Material>> materials;
  HittableList world;

  Lambertian ground_material{Color{0.5, 0.5, 0.5}};
  Sphere ground_sphere{Point3{0, -1000, 0}, 1000, &ground_material};
  world.Add(&ground_sphere);

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
        } else if (choose_mat < 0.95) {
          // Metal.
          Color albedo = Color::Random(0.5, 1);
          Float fuzz = RandomFloat(0, 0.5);
          materials.push_back(std::make_unique<Metal>(albedo, fuzz));
        } else {
          // Glass.
          materials.push_back(std::make_unique<Dielectric>(1.5));
        }

        geometries.push_back(
            std::make_unique<Sphere>(center, 0.2, materials.back().get()));
        world.Add(geometries.back().get());
      }
    }
  }

  Dielectric material1{1.5};
  Sphere sphere1{Point3{0, 1, 0}, 1.0, &material1};
  world.Add(&sphere1);

  Lambertian material2{Color{0.4, 0.2, 0.1}};
  Sphere sphere2{Point3{-4, 1, 0}, 1.0, &material2};
  world.Add(&sphere2);

  Metal material3{Color{0.7, 0.6, 0.5}, 0.0};
  Sphere sphere3{Point3{4, 1, 0}, 1.0, &material3};
  world.Add(&sphere3);

  AppSettings settings{
      .window_width = 1280,
      .window_height = 720,

      .enable_rendering = true,

      .image_scale_factor = 0.5f,
      .samples_per_pixel_log2 = 5,
      .max_depth_log2 = 3,
      .fov = 20.0f,
      .look_from = {13.0f, 2.0f, 3.0f},
      .look_at = {0.0f, 0.0f, 0.0f},
      .view_up = {0.0f, 1.0f, 0.0f},
      .defocus_angle = 0.6f,
      .focus_distance = 10.0f,
  };
  App app{settings, world};
  app.Run();

  return 0;
}