#pragma once

#include "vec3.h"
#include "shape3.h"

class camera {
public:
  camera(
    point3 look_from,    // point on camera plane
    point3 look_at,      // point on image plane
    vec3 v_up,           // reference vector pointing "up"
    double vert_fov,     // vertical FOV in radians
    double aspect_ratio,
    double aperture,
    double focus_dist    // distance to virtual plane w/ everything is in focus
  ) {
    // Using Thin Lens approximation
    auto viewport_height = 2.0 * tan(vert_fov / 2);
    auto viewport_width = aspect_ratio * viewport_height;

    w = unit_vec3(look_from - look_at);
    u = unit_vec3(cross(v_up, w));  // perpindicular to camera plane
    v = cross(w, u);  // "up" on the camera plane

    origin = look_from;
    horizontal = focus_dist * viewport_width * u;
    vertical = focus_dist * viewport_height * v;
    lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

    lens_radius = aperture / 2;
  }

  ray get_ray(double s, double t) const {
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u*rd.x() + v*rd.y();
    ray r = ray(origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset);
    return r;
  }

private:
  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  double lens_radius;
};
