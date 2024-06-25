#include "dielectric.h"

#include <cmath>
#include <optional>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"
#include "vec3.h"

std::optional<ScatterRecord> Dielectric::Scatter(
    const Ray& ray, const HitRecord& record) const {
  const Float refraction_index =
      record.is_front_face() ? (1.0 / refraction_index_) : refraction_index_;

  const Vec3 unit_direction = UnitVector(ray.direction());
  const Float cos_theta = std::fmin(Dot(-unit_direction, record.normal()), 1.0);
  const Float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

  const bool cannot_refract = refraction_index * sin_theta > 1.0;
  const bool is_reflective =
      Reflectance(cos_theta, refraction_index) > RandomFloat();
  const Vec3 direction =
      cannot_refract || is_reflective
          ? Reflect(unit_direction, record.normal())
          : Refract(unit_direction, record.normal(), refraction_index);

  const Color attenuation{1.0, 1.0, 1.0};
  const Ray scattered{record.p(), direction};
  return ScatterRecord{attenuation, scattered};
}