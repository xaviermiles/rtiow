#pragma once

struct hit_record;

class material {
public:
  virtual bool scatter(
    const ray& r_in, const hit_record& rec, colour& attenuation, ray& r_out
  ) const = 0;
};

class lambertian : public material {
  // Perfectly diffuse
  // attenuation: "the reduction of the force, effect, or value of something"
  // albedo: "the proportion of the incident light or radiation that is reflected by a surface"
public:
  lambertian(const colour& a) : albedo(a) {}

  virtual bool scatter(
    const ray& r_in, const hit_record& rec, colour& attenuation, ray& r_out
  ) const override {
    vec3 scatter_direction = rec.normal + random_unit_vec3();

    // Catch degenerate scatter direction ie. random_unit_vec3() = -rec.normal
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    r_out = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

public:
  colour albedo;
};

class metal : public material {
  // Perflectly reflective, with some fuzziness
public:
  metal(const colour& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(
    const ray& r_in, const hit_record& rec, colour& attenuation, ray& r_out
  ) const override {
    vec3 reflected = reflect(unit_vec3(r_in.direction()), rec.normal);
    r_out = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenuation = albedo;
    return (dot(r_out.direction(), rec.normal) > 0);
  }

public:
  colour albedo;
  double fuzz;
};

class dielectric : public material {
  // Refractive glass
public:
  dielectric(double index_of_refraction) : refr_idx(index_of_refraction) {}

  virtual bool scatter(
    const ray& r_in, const hit_record& rec, colour& attenuation, ray& r_out
  ) const override {
    attenuation = colour(1.0, 1.0, 1.0);
    double refr_idx_air = 1.0; // approximately
    double refr_ratio = rec.front_face ? (refr_idx_air / refr_idx)
                                       : (refr_idx / refr_idx_air);

    vec3 unit_direction = unit_vec3(r_in.direction());
    double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    // If the ray cannot refract (adjusting for angle of surface), then total
    // internal reflection
    bool can_refract = (refr_ratio * sin_theta) <= 1.0;
    vec3 out_direction;
    if (can_refract)// || reflectance(cos_theta, refr_ratio) > random_double())
      out_direction = refract(unit_direction, rec.normal, refr_ratio);
    else
      out_direction = reflect(unit_direction, rec.normal);

    r_out = ray(rec.p, out_direction);
    return true;
  }

public:
  double refr_idx;

private:
  static double reflectance(double cos, double refr_idx) {
    // Schlick's polynomial approximation for reflectance.
    double r0 = pow((1 - refr_idx) / (1 + refr_idx), 2);
    double refl = r0 + (1 - r0)*pow(1 - cos, 5);
    return refl;
  }
};
