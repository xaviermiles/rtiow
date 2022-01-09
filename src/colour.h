#pragma once

#include "vec3.h"
#include "ray.h"
#include "utils.h"

#include <iostream>

void write_colour(std::ostream &out, colour pixel_colour, int samples_per_pixel) {
  // Divide the color by the number of samples and gamma-correct for gamma=2.0.
  auto scaled_pixel_colour = pixel_colour / samples_per_pixel;

  auto r = sqrt(scaled_pixel_colour.x());
  auto g = sqrt(scaled_pixel_colour.y());
  auto b = sqrt(scaled_pixel_colour.z());

  // Write in [0, 256) range
  out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
