#include "lambertian.h"

#include <optional>

#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

std::optional<ScatterRecord> Lambertian::Scatter(
    const Ray& ray, const HitRecord& record) const {
  Vec3 scatter_direction = record.normal() + RandomUnitVector();
  if (scatter_direction.near_zero()) {
    scatter_direction = record.normal();
  }

  const Ray scattered{record.p(), scatter_direction};
  return ScatterRecord{albedo_, scattered};
}