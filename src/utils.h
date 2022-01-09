#pragma once

#include <cstdlib>

// Misc functions

inline double random_double() {
  // Return a random double in [0, 1)
  return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random double in [min, max)
  return min + (max - min)*random_double();
}

inline double clamp(double x, double min, double max) {
  // Return the closest number to x in range [min, max]
  if (x < min)
    return min;
  else if (x > max)
    return max;

  return x;
}
