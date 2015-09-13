#pragma once
#include "pupumath_struct.hpp"
#include "util.hpp"
#include <memory>

struct Sampler;

struct Sample {
  Sampler* sampler;
  int id;
  int shading_counter;

  Sample(Sampler* sampler, int id)
      : sampler(sampler), id(id), shading_counter(0)
  {
  }

  float wavelen() const;
  pupumath::vec2 lens() const;
  pupumath::vec2 shading();
};

struct Sampler {
  int n;

  Sampler(int n) : n(n) {}
  virtual ~Sampler() {}

  /// Generate samples for one pixel.
  virtual void generate() = 0;

  virtual float get_wavelen(int sample_id) = 0;
  virtual pupumath::vec2 get_lens(int sample_id) = 0;
  virtual pupumath::vec2 get_shading(int sample_id, int counter) = 0;
};

inline float Sample::wavelen() const { return sampler->get_wavelen(id); }
inline pupumath::vec2 Sample::lens() const { return sampler->get_lens(id); }
inline pupumath::vec2 Sample::shading()
{
  return sampler->get_shading(id, shading_counter++);
}

std::shared_ptr<Sampler> create_sampler(int n, const std::string& name);
