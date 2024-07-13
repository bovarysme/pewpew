#ifndef PEWPEW_UTILS_H_
#define PEWPEW_UTILS_H_

#include <numbers>
#include <random>

#include "float.h"
#include "pcg_random.hpp"

inline Float DegreesToRadians(Float degrees) {
  return degrees * std::numbers::pi / 180.0;
}

inline Float RandomFloat() {
  thread_local pcg_extras::seed_seq_from<std::random_device> seed_source;
  thread_local pcg32 rng(seed_source);
  return rng() / (rng.max() + 1.0);
}

inline Float RandomFloat(double min, double max) {
  return min + (max - min) * RandomFloat();
}

#endif  // PEWPEW_UTILS_H_