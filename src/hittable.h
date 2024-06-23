#ifndef PEWPEW_HITTABLE_H_
#define PEWPEW_HITTABLE_H_

#include <optional>

#include "float.h"
#include "ray.h"
#include "vec3.h"

class HitRecord {
 public:
  HitRecord(Float t, const Point3& p, const Vec3& outward_normal,
            const Ray& ray)
      : t_(t), p_(p) {
    is_front_face_ = Dot(ray.direction(), outward_normal) < 0;
    normal_ = is_front_face_ ? outward_normal : -outward_normal;
  }

  Float t() { return t_; }
  Point3 p() { return p_; }
  Vec3 normal() { return normal_; }

 private:
  Float t_;
  Point3 p_;
  bool is_front_face_;
  Vec3 normal_;
};

class Hittable {
 public:
  virtual ~Hittable() = default;

  virtual std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                                       Float tmax) const = 0;
};

#endif  // PEWPEW_HITTABLE_H_