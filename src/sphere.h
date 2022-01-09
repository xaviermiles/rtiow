#pragma once

#include "shape3.h"
#include "vec3.h"

class sphere : public shape3 {
public:
  sphere() {}
  sphere(point3 c, double r, shared_ptr<material> m)
    : centre(c), radius(r), mat_ptr(m) {}

  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
  point3 centre;
  double radius;
  shared_ptr<material> mat_ptr;
  bool front_face;  // better name?
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  vec3 r_origin_d = r.origin() - centre;
  auto a = r.direction().length_squared();
  auto b = 2.0 * dot(r.direction(), r_origin_d);
  auto c = r_origin_d.length_squared() - radius*radius;

  auto discriminant = b*b - 4*a*c;
  if (discriminant < 0)
    return false;
  auto sqrt_D = sqrt(discriminant);

  // Find the nearest root lying in the acceptable range.
  auto root = (-b - sqrt_D) / (2.0 * a);
  if (root < t_min || t_max < root) {
    root = (-b + sqrt_D) / (2.0 * a);
    if (root < t_min || t_max < root)
      return false;  // neither roots are in acceptable range
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  vec3 outward_normal = (rec.p - centre) / radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}
