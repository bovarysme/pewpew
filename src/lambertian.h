#ifndef PEWPEW_LAMBERTIAN_H_
#define PEWPEW_LAMBERTIAN_H_

#include <optional>

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"

class Lambertian : public Material {
 public:
  Lambertian(const Color& albedo) : albedo_(albedo) {}

  std::optional<ScatterRecord> Scatter(const Ray& ray,
                                       const HitRecord& record) const override;

 private:
  Color albedo_;
};

#endif  // PEWPEW_LAMBERTIAN_H_