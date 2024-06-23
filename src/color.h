#ifndef PEWPEW_COLOR_H_
#define PEWPEW_COLOR_H_

#include <algorithm>
#include <iostream>

#include "float.h"
#include "vec3.h"

using Color = Vec3;

inline void WriteColor(std::ostream& out, const Color& pixel_color) {
  const Float r = pixel_color.x();
  const Float g = pixel_color.y();
  const Float b = pixel_color.z();

  const Float min = 0.0;
  const Float max = 0.999;
  const int r_byte = static_cast<int>(256 * std::clamp(r, min, max));
  const int g_byte = static_cast<int>(256 * std::clamp(g, min, max));
  const int b_byte = static_cast<int>(256 * std::clamp(b, min, max));

  out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

#endif  // PEWPEW_COLOR_H_