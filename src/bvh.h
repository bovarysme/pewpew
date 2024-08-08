#ifndef PEWPEW_BVH_H_
#define PEWPEW_BVH_H_

#include <optional>
#include <vector>

#include "aabb.h"
#include "float.h"
#include "hittable.h"
#include "hittable_list.h"
#include "ray.h"

class BVHNode : public Hittable {
 public:
  BVHNode(HittableList list)
      : BVHNode{list.objects(), 0, list.objects().size()} {};

  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

  AABB BoundingBox() const override { return bounding_box_; }

 private:
  BVHNode(std::vector<Hittable*>& objects, size_t start, size_t end);

  Hittable* left_;
  Hittable* right_;
  AABB bounding_box_;
};

class BoxComparator {
 public:
  BoxComparator(int axis) : axis_(axis) {}

  bool operator()(Hittable* h1, Hittable* h2) {
    return h1->BoundingBox().min()[axis_] < h2->BoundingBox().min()[axis_];
  }

 private:
  int axis_;
};

#endif  // PEWPEW_BVH_H_