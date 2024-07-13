#ifndef PEWPEW_HITTABLE_LIST_H_
#define PEWPEW_HITTABLE_LIST_H_

#include <memory>
#include <optional>
#include <vector>

#include "aabb.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"

class HittableList : public Hittable {
 public:
  void Add(std::shared_ptr<Hittable> object) {
    objects_.push_back(object);
    bounding_box_ = AABB{bounding_box_, object->BoundingBox()};
  }

  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

  AABB BoundingBox() const override { return bounding_box_; }

 private:
  std::vector<std::shared_ptr<Hittable>> objects_;
  AABB bounding_box_;
};

#endif  // PEWPEW_HITTABLE_LIST_H_