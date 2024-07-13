#ifndef PEWPEW_SPHERE_H_
#define PEWPEW_SPHERE_H_

#include <memory>
#include <optional>

#include "aabb.h"
#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

class Sphere : public Hittable {
 public:
  Sphere(const Point3& center, Float radius, Material* material)
      : center_(center), radius_(radius), material_(material) {
    Vec3 bounding_box_half_size = Vec3{radius, radius, radius};
    bounding_box_ =
        AABB{center - bounding_box_half_size, center + bounding_box_half_size};
  }

  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

  AABB BoundingBox() const override { return bounding_box_; }

 private:
  Point3 center_;
  Float radius_;
  Material* material_;
  AABB bounding_box_;
};

#endif  // PEWPEW_SPHERE_H_