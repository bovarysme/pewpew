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
  Lambertian material_ground{Color{0.8, 0.8, 0.0}};
  Lambertian material_center{Color{0.1, 0.2, 0.5}};
  Dielectric material_left{1.5};
  Dielectric material_bubble{1.0 / 1.5};
  Metal material_right{Color{0.8, 0.6, 0.2}, 1.0};

  HittableList world;
  world.Add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0,
                                     &material_ground));
  world.Add(
      std::make_shared<Sphere>(Point3{0.0, 0.0, -1.2}, 0.5, &material_center));
  world.Add(
      std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.5, &material_left));
  world.Add(
      std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.4, &material_bubble));
  world.Add(
      std::make_shared<Sphere>(Point3{1.0, 0.0, -1.0}, 0.5, &material_right));

  CameraSettings settings{
      .image_width = 640,
      .image_height = 360,
      .samples_per_pixel = 128,
      .max_depth = 16,
      .fov = 30,
      .look_from = Point3{-2, 2, 1},
      .look_at = Point3{0, 0, -1},
      .view_up = Vec3{0, 1, 0},
      .defocus_angle = 10.0,
      .focus_distance = 3.4,
  };

  Camera camera{settings};
  camera.Render(world);
}