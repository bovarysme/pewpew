#include <memory>

#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "vec3.h"

int main() {
  HittableList world;
  world.Add(std::make_shared<Sphere>(Point3{0, 0, -1}, 0.5));
  world.Add(std::make_shared<Sphere>(Point3{0.5, 0.5, -1}, 0.25));
  world.Add(std::make_shared<Sphere>(Point3{-0.5, 0.5, -1}, 0.25));
  world.Add(std::make_shared<Sphere>(Point3{0, -100.5, -1}, 100));

  Camera camera{16.0 / 9.0, 400};
  camera.Render(world);
}