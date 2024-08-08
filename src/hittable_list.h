#ifndef PEWPEW_HITTABLE_LIST_H_
#define PEWPEW_HITTABLE_LIST_H_

#include <optional>
#include <vector>

#include "aabb.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"

class HittableList : public Hittable {
 public:
  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

  AABB BoundingBox() const override { return bounding_box_; }

  void Add(Hittable* object) {
    objects_.push_back(object);
    bounding_box_ = AABB{bounding_box_, object->BoundingBox()};
  }

  std::vector<Hittable*> objects() const { return objects_; }

 private:
  std::vector<Hittable*> objects_;
  AABB bounding_box_;
};

#endif  // PEWPEW_HITTABLE_LIST_H_