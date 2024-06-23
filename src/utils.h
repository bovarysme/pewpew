#ifndef PEWPEW_UTILS_H_
#define PEWPEW_UTILS_H_

#include <cstdlib>

inline double RandomDouble() { return rand() / (RAND_MAX + 1.0); }

#endif  // PEWPEW_UTILS_H_