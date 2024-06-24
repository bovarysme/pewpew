#ifndef PEWPEW_MATERIAL_H_
#define PEWPEW_MATERIAL_H_

#include <optional>

#include "color.h"
#include "hittable.h"
#include "ray.h"

class HitRecord;

class ScatterRecord {
 public:
  ScatterRecord(const Color& attenuation, const Ray& scattered)
      : attenuation_(attenuation), scattered_(scattered) {}

  Color attenuation() const { return attenuation_; }
  Ray scattered() const { return scattered_; }

 private:
  Color attenuation_;
  Ray scattered_;
};

class Material {
 public:
  virtual ~Material() = default;

  virtual std::optional<ScatterRecord> Scatter(
      const Ray& ray, const HitRecord& record) const = 0;
};

#endif  // PEWPEW_MATERIAL_H_