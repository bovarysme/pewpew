#include "aabb.h"

#include <algorithm>
#include <utility>

#include "ray.h"
#include "vec3.h"

bool AABB::Hit(const Ray& ray, float tmin, float tmax) const {
  const Point3& origin = ray.origin();
  const Vec3& direction = ray.direction();

  for (int axis = 0; axis < 3; axis++) {
    float t0 = (min_[axis] - origin[axis]) / direction[axis];
    float t1 = (max_[axis] - origin[axis]) / direction[axis];
    if (t0 >= t1) {
      std::swap(t0, t1);
    }

    tmin = std::max(tmin, t0);
    tmax = std::min(tmax, t1);
    if (tmax <= tmin) {
      return false;
    }
  }

  return true;
}