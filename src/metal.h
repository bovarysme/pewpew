#ifndef PEWPEW_METAL_H_
#define PEWPEW_METAL_H_

#include <optional>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"

class Metal : public Material {
 public:
  Metal(const Color& albedo, Float fuzz) : albedo_(albedo), fuzz_(fuzz) {}

  std::optional<ScatterRecord> Scatter(const Ray& ray,
                                       const HitRecord& record) const override;

 private:
  Color albedo_;
  Float fuzz_;
};

#endif  // PEWPEW_METAL_H_