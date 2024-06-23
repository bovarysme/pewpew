#ifndef PEWPEW_COLOR_H_
#define PEWPEW_COLOR_H_

#include <algorithm>
#include <cmath>
#include <iostream>

#include "float.h"
#include "vec3.h"

using Color = Vec3;

inline Float LinearToGamma(double linear_component) {
  return linear_component > 0 ? std::sqrt(linear_component) : 0;
}

inline void WriteColor(std::ostream& out, const Color& pixel_color) {
  Float r = pixel_color.x();
  Float g = pixel_color.y();
  Float b = pixel_color.z();

  r = LinearToGamma(r);
  g = LinearToGamma(g);
  b = LinearToGamma(b);

  const Float min = 0.0;
  const Float max = 0.999;
  const int r_byte = static_cast<int>(256 * std::clamp(r, min, max));
  const int g_byte = static_cast<int>(256 * std::clamp(g, min, max));
  const int b_byte = static_cast<int>(256 * std::clamp(b, min, max));

  out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

#endif  // PEWPEW_COLOR_H_