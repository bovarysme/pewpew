#include <memory>

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "sphere.h"
#include "vec3.h"

int main() {
  Lambertian material_ground{Color{0.8, 0.8, 0.0}};
  Lambertian material_center{Color{0.1, 0.2, 0.5}};
  Metal material_left{Color{0.8, 0.8, 0.8}, 0.3};
  Metal material_right{Color{0.8, 0.6, 0.2}, 1.0};

  HittableList world;
  world.Add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0,
                                     &material_ground));
  world.Add(
      std::make_shared<Sphere>(Point3{0.0, 0.0, -1.2}, 0.5, &material_center));
  world.Add(
      std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.5, &material_left));
  world.Add(
      std::make_shared<Sphere>(Point3{1.0, 0.0, -1.0}, 0.5, &material_right));

  Camera camera{640, 360, 128, 8};
  camera.Render(world);
}