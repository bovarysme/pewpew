#include "metal.h"

#include <optional>

#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

std::optional<ScatterRecord> Metal::Scatter(const Ray& ray,
                                            const HitRecord& record) const {
  Vec3 reflection_direction = Reflect(ray.direction(), record.normal());
  reflection_direction =
      UnitVector(reflection_direction) + (fuzz_ * RandomUnitVector());
  if (Dot(reflection_direction, record.normal()) <= 0) {
    return std::nullopt;
  }

  const Ray scattered{record.p(), reflection_direction};
  return ScatterRecord{albedo_, scattered};
}