#pragma once
#include "pupumath_struct.hpp"
#include <memory>

class ValueBlock;

class Skybox {
public:
  virtual ~Skybox() {}
  virtual float sample(const pupumath::vec3 &dir, float wavelen) = 0;
};

std::shared_ptr<Skybox> build_skybox(const ValueBlock &);
