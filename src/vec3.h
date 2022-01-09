#pragma once

#include "utils.h"

#include <cmath>
#include <iostream>

using std::abs;
using std::sqrt;

class vec3 {
public:
  vec3(): e{0, 0, 0} {}
  vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }

  vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
  double operator[](int i) const { return e[i]; }
  double& operator[](int i) { return e[i]; }

  vec3& operator+=(const vec3 &v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  vec3& operator-=(vec3 &v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
  }

  vec3& operator*=(const double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  vec3& operator/=(const double t) {
    return *this *= 1/t;
  }

  double length() const {
    return sqrt(length_squared());
  }

  double length_squared() const {
    return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
  }

  inline static vec3 random() {
    return vec3(random_double(), random_double(), random_double());
  }

  inline static vec3 random(double min, double max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
  }

  bool near_zero() const {
    // Return true if the vector is "close" to the zero vector.
    const auto s = 1e-8;
    return (abs(e[0]) < s) && (abs(e[1]) < s) && (abs(e[2]) < s);
  }

public:
  double e[3];
};

// Type aliases for vec3
using point3 = vec3;  // 3d point
using colour = vec3;   // rgb colour

// Operators
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
  return vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
  return vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
  return vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline vec3 operator*(double t, const vec3 &u) {
  return vec3(t * u.x(), t * u.y(), t * u.z());
}
inline vec3 operator*(const vec3 &u, double t) { return t * u; }

inline vec3 operator/(vec3 u, double t) {
  return (1 / t) * u;
}

inline bool operator==(const vec3 &u, const vec3 &v) {
  return u.e[0]==v.e[0] && u.e[1]==v.e[1] && u.e[2]==v.e[2];
}

inline double dot(const vec3 &u, const vec3 &v) {
  return u.e[0]*v.e[0] + u.e[1]*v.e[1] + u.e[2]*v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
  return vec3(
    u.e[1]*v.e[2] - u.e[2]*v.e[1],
    -(u.e[0]*v.e[2] - u.e[2]*v.e[0]),
    u.e[0]*v.e[1] - u.e[1]*v.e[0]
  );
}

inline vec3 unit_vec3(vec3 u) {
  return u / u.length();
}

// Utility functions
vec3 random_in_unit_sphere() {
  while (true) {
    auto p = vec3::random(-1, 1);
    if (p.length_squared() < 1)
      return p;
  }
}

vec3 random_in_unit_disk() {
  while (true) {
    auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1)
      return p;
  }
}

vec3 random_unit_vec3() {
  return unit_vec3(random_in_unit_sphere());
}

vec3 random_in_hemisphere(const vec3& normal) {
  vec3 in_unit_sphere = random_in_unit_sphere();
  if (dot(in_unit_sphere, normal) > 0.0)  // In the same hemisphere as the normal
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2*dot(v, n)*n;
}

vec3 refract(const vec3& u, const vec3& n, double refr_idxs_ratio) {
  auto cos_theta = std::min(dot(-u, n), 1.0);
  vec3 r_out_perp = refr_idxs_ratio * (u + cos_theta*n);
  vec3 r_out_para = -n * sqrt(abs(1.0 - r_out_perp.length_squared()));
  vec3 r_out = r_out_perp + r_out_para;
  return r_out;
}
