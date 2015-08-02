#pragma once
#include "pupumath_struct.hpp"
#include <memory>

struct Pixel {
  pupumath::vec3 value;
  float weight;

  Pixel();

  pupumath::vec3 normalized() const;
};

class Framebuffer {
public:
  int xres, yres;
  std::unique_ptr<Pixel[]> pixels;

  Framebuffer(int xres, int yres);

  void add_sample(float x, float y, const pupumath::vec3 &v);
  void save_ppm();
};
