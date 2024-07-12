#ifndef PEWPEW_COLOR_H_
#define PEWPEW_COLOR_H_

#include <algorithm>
#include <cmath>

#include "float.h"
#include "vec3.h"

using Color = Vec3;

inline Float LinearToGamma(double linear_component) {
  return linear_component > 0 ? std::sqrt(linear_component) : 0;
}

inline uint8_t TransformColor(Float color) {
  color = LinearToGamma(color);

  const Float min = 0.0;
  const Float max = 0.999;
  return static_cast<uint8_t>(256 * std::clamp(color, min, max));
}

#endif  // PEWPEW_COLOR_H_