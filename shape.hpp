#pragma once
#include <memory>

struct Ray;
struct ValueBlock;

class Shape {
public:
  virtual bool intersect(Ray &ray, bool is_originator,
                         bool inside_originator) const = 0;
};

std::shared_ptr<Shape> build_shape (const ValueBlock&);

