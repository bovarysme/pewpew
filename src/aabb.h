#ifndef PEWPEW_AABB_H_
#define PEWPEW_AABB_H_

#include "ray.h"
#include "vec3.h"

class AABB {
 public:
  AABB() {}
  AABB(const Point3& p1, const Point3& p2)
      : min_{Min(p1, p2)}, max_{Max(p1, p2)} {}
  AABB(const AABB& b1, const AABB& b2)
      : min_{Min(b1.min_, b2.min_)}, max_{Max(b1.max_, b2.max_)} {}

  bool Hit(const Ray& ray, float tmin, float tmax) const;

 private:
  Point3 min_;
  Point3 max_;
};

#endif  // PEWPEW_AABB_H_