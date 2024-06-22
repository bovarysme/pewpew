#include "hittable_list.h"

#include <optional>

#include "float.h"
#include "hittable.h"
#include "ray.h"

std::optional<HitRecord> HittableList::Hit(const Ray& ray, Float tmin,
                                           Float tmax) const {
  Float closest = tmax;
  std::optional<HitRecord> record = std::nullopt;

  for (const auto& object : objects_) {
    std::optional<HitRecord> temp_record = object->Hit(ray, tmin, closest);
    if (temp_record.has_value()) {
      closest = temp_record->t();
      record = temp_record;
    }
  }

  return record;
}