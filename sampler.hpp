#pragma once
#include "pupumath_struct.hpp"
#include <memory>

struct Sampler {
  int n;
  std::unique_ptr<float[]> wavelen;
  std::unique_ptr<pupumath::vec3[]> lens;
  std::unique_ptr<pupumath::vec3[]> shading;

  Sampler(int n);
  void generate();
  void next();
};
