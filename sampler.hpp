#pragma once
#include "pupumath_struct.hpp"
#include "util.hpp"
#include <memory>

struct Sampler {
  int n;
  std::unique_ptr<float[]> wavelen;
  std::unique_ptr<pupumath::vec2[]> lens;
  std::unique_ptr<pupumath::vec2[]> shading;

  Sampler(int n);
  void generate();
  void next();
};

struct Sample {
  Sampler* sampler;
  int sample_index;
  int shading_counter;

  float wavelen() const { return sampler->wavelen[sample_index]; }
  pupumath::vec2 lens() const { return sampler->lens[sample_index]; }
  pupumath::vec2 shading()
  {
    ++shading_counter;
    if (shading_counter <= 4) {
      return sampler
          ->shading[(shading_counter - 1) * sampler->n + sample_index];
    }
    else {
      return pupumath::vec2(frand(), frand());
    }
  }
};

std::shared_ptr<Sampler> create_sampler(int n, const std::string& name);
