#ifndef PEWPEW_HITTABLE_LIST_H_
#define PEWPEW_HITTABLE_LIST_H_

#include <memory>
#include <optional>
#include <vector>

#include "float.h"
#include "hittable.h"
#include "ray.h"

class HittableList : public Hittable {
 public:
  void Add(std::shared_ptr<Hittable> object) { objects_.push_back(object); }

  std::optional<HitRecord> Hit(const Ray& ray, Float tmin,
                               Float tmax) const override;

 private:
  std::vector<std::shared_ptr<Hittable>> objects_;
};

#endif  // PEWPEW_HITTABLE_LIST_H_