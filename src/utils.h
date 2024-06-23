#ifndef PEWPEW_UTILS_H_
#define PEWPEW_UTILS_H_

#include <cstdlib>

#include "float.h"

inline Float RandomFloat() { return rand() / (RAND_MAX + 1.0); }

inline Float RandomFloat(double min, double max) {
  return min + (max - min) * RandomFloat();
}

#endif  // PEWPEW_UTILS_H_