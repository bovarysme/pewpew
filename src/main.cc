#include <memory>

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "sphere.h"
#include "vec3.h"

int main() {
  Lambertian material_ground{Color{0.8, 0.8, 0.8}};
  Lambertian material_center{Color{0.1, 0.2, 0.5}};

  HittableList world;
  world.Add(std::make_shared<Sphere>(Point3{0, 0, -1}, 0.5, &material_center));
  world.Add(
      std::make_shared<Sphere>(Point3{0.5, 0.5, -1}, 0.25, &material_center));
  world.Add(
      std::make_shared<Sphere>(Point3{-0.5, 0.5, -1}, 0.25, &material_center));
  world.Add(
      std::make_shared<Sphere>(Point3{0, -100.5, -1}, 100, &material_ground));

  Camera camera{640, 360, 128, 64};
  camera.Render(world);
}