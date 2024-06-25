#ifndef PEWPEW_DIELECTRIC_H_
#define PEWPEW_DIELECTRIC_H_

#include <cmath>
#include <optional>

#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"

class Dielectric : public Material {
 public:
  Dielectric(Float refraction_index) : refraction_index_(refraction_index) {}

  std::optional<ScatterRecord> Scatter(const Ray& ray,
                                       const HitRecord& record) const override;

 private:
  Float refraction_index_;
};

inline Float Reflectance(Float cosine, Float refraction_index) {
  Float r0 = (1 - refraction_index) / (1 + refraction_index);
  r0 = r0 * r0;
  return r0 + (1 - r0) * std::pow(1 - cosine, 5);
}

#endif  // PEWPEW_DIELECTRIC_H_