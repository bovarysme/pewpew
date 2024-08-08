#include "bvh.h"

#include <algorithm>
#include <optional>
#include <vector>

#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "utils.h"

std::optional<HitRecord> BVHNode::Hit(const Ray& ray, Float tmin,
                                      Float tmax) const {
  if (!bounding_box_.Hit(ray, tmin, tmax)) {
    return std::nullopt;
  }

  std::optional<HitRecord> hit_left = left_->Hit(ray, tmin, tmax);
  Float closest = hit_left.has_value() ? hit_left->t() : tmax;
  std::optional<HitRecord> hit_right = right_->Hit(ray, tmin, closest);

  if (hit_right.has_value()) {
    return hit_right;
  }
  if (hit_left.has_value()) {
    return hit_left;
  }
  return std::nullopt;
}

BVHNode::BVHNode(std::vector<Hittable*>& objects, size_t start, size_t end) {
  size_t span = end - start;
  if (span == 1) {
    left_ = objects[start];
    right_ = objects[start];
  } else if (span == 2) {
    left_ = objects[start];
    right_ = objects[start + 1];
  } else {
    int axis = RandomInt(0, 2);
    std::sort(objects.begin() + start, objects.begin() + end,
              BoxComparator(axis));

    size_t mid = start + span / 2;
    left_ = BVHNode{objects, start, mid};
    right_ = BVHNode{objects, mid, end};
  }

  bounding_box_ = AABB{left_->BoundingBox(), right_->BoundingBox()};
}