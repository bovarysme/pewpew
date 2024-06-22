#ifndef PEWPEW_SPHERE_H_
#define PEWPEW_SPHERE_H_

#include <optional>

#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

class Sphere : public Hittable {
 public:
  Sphere(const Point3& center, Float radius)
      : center_(center), radius_(radius) {}

  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

 private:
  Point3 center_;
  Float radius_;
};

#endif  // PEWPEW_SPHERE_H_