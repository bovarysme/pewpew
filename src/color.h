#ifndef PEWPEW_COLOR_H_
#define PEWPEW_COLOR_H_

#include <iostream>

#include "vec3.h"

using Color = Vec3;

void WriteColor(std::ostream& out, const Color& pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  int r_byte = static_cast<int>(255.999 * r);
  int g_byte = static_cast<int>(255.999 * g);
  int b_byte = static_cast<int>(255.999 * b);

  out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

#endif  // PEWPEW_COLOR_H_