#include "sphere.h"

#include <cmath>
#include <optional>

#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

std::optional<HitRecord> Sphere::Hit(const Ray& ray, Float tmin,
                                     Float tmax) const {
  const Vec3 origin_to_center = center_ - ray.origin();
  const Float a = ray.direction().length_squared();
  const Float h = Dot(ray.direction(), origin_to_center);
  const Float c = origin_to_center.length_squared() - radius_ * radius_;

  const Float discriminant = h * h - a * c;
  if (discriminant < 0) {
    return std::nullopt;
  }

  const Float discriminant_sqrt = std::sqrt(discriminant);

  Float root = (h - discriminant_sqrt) / a;
  if (root <= tmin || root >= tmax) {
    root = (h + discriminant_sqrt) / a;
    if (root <= tmin || root >= tmax) {
      return std::nullopt;
    }
  }

  const Point3 intersection = ray.at(root);
  const Vec3 outward_normal = (intersection - center_) / radius_;
  return HitRecord{root, intersection, outward_normal, ray};
}