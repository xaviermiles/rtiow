#pragma once

#include "shape3.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class shape3_list : public shape3 {
public:
  shape3_list() {}
  shape3_list(shared_ptr<shape3> object) { add(object); }

  void clear() { objects.clear(); }
  void add(shared_ptr<shape3> object) { objects.push_back(object); }

  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
  std::vector<shared_ptr<shape3>> objects;
};

bool shape3_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;

  for (const auto& object : objects) {
    if (object->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}
