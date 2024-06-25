#ifndef PEWPEW_UTILS_H_
#define PEWPEW_UTILS_H_

#include <numbers>
#include <random>

#include "../third_party/pcg-cpp/include/pcg_random.hpp"
#include "float.h"

inline Float DegreesToRadians(Float degrees) {
  return degrees * std::numbers::pi / 180.0;
}

inline Float RandomFloat() {
  static pcg_extras::seed_seq_from<std::random_device> seed_source;
  static pcg32 rng(seed_source);
  return rng() / (rng.max() + 1.0);
}

inline Float RandomFloat(double min, double max) {
  return min + (max - min) * RandomFloat();
}

#endif  // PEWPEW_UTILS_H_