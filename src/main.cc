#include "camera.h"
#include "colour.h"
#include "material.h"
#include "ray.h"
#include "shape3.h"
#include "shape3_list.h"
#include "sphere.h"
#include "utils.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const double _INFINITY = std::numeric_limits<double>::infinity();
const double _PI = 3.1415926535897932385;


colour ray_colour(const ray& r, const shape3& world, int bounces) {
  hit_record rec;

  // If exceeded the depth/bounce limit, no more light is gathered.
  if (bounces <= 0)
    return colour(0, 0, 0);

  double show_acne_threshold = 0.001;
  if (world.hit(r, show_acne_threshold, _INFINITY, rec)) {
    ray scattered;
    colour attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_colour(scattered, world, bounces - 1);
    return colour(0, 0, 0);
  }

  // Background is linearly interpolated white/blue, based on unit-y coord
  colour white = colour(1.0, 1.0, 1.0);
  colour blue = colour(0.5, 0.7, 1.0);
  vec3 unit_direction = unit_vec3(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t)*white + t*blue;
}

shape3_list random_scene() {
  shape3_list world;

  auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
  auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
  auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      double choose_mat = random_double();
      point3 centre(a + random_double(0, 0.9), 0.2, b + random_double(0, 0.9));

      if ((centre - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<material> sphere_mat;

        if (choose_mat < 0.8) {
          // lambertian
          colour albedo = colour::random() * colour::random();
          sphere_mat = make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(centre, 0.2, sphere_mat));
        } else if (choose_mat < 0.95) {
          // metal
          colour albedo = colour::random(0.5, 1);
          double fuzz = random_double(0, 0.5);
          sphere_mat = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(centre, 0.2, sphere_mat));
        } else {
          // glass
          sphere_mat = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(centre, 0.2, sphere_mat));
        }
      }
    }
  }

  return world;
}

int main() {
  auto start_t = std::chrono::high_resolution_clock::now();

  // Image - TWEAK THESE SETTINGS FOR HIGHER FIDALITY etc.
  const double aspect_ratio = 3.0 / 2.0;
  const int image_width = 1200;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 20;
  const int max_bounces = 8;

  // World
  shape3_list world = random_scene();

  // Camera
  const point3 look_from(13, 2, 3);
  const point3 look_at(0, 0, 0);
  const vec3 v_up(0, 1, 0);
  const double vert_fov = _PI / 9;
  const double dist_to_focus = 10.0;
  const double aperture = 0.1;
  camera cam(look_from, look_at, v_up, vert_fov, aspect_ratio, aperture, dist_to_focus);

  // Render
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      // Antialiasing: getting pixel from average of samples
      colour pixel_colour(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_colour += ray_colour(r, world, max_bounces);
      }
      write_colour(std::cout, pixel_colour, samples_per_pixel);
    }
  }

  auto end_t = std::chrono::high_resolution_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_t - start_t);
  std::cerr << "\nCompleted in " << time_elapsed.count() << "s.\n";
}
