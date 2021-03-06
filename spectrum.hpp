#pragma once
#include "pupumath_struct.hpp"
#include <array>

struct Spectrum {
  static constexpr float min = 400;
  static constexpr float max = 700;
  static constexpr int count = 16;
  static constexpr float count_over_max_minus_min = count / (max - min);
  static constexpr float wavelen(float f) { return min + (max - min) * f; }

  std::array<float, count> samples;

  float sample(float wavelen) const
  {
    if (wavelen < min || wavelen >= max) return 0.0f;
    return samples[int((wavelen - min) * count_over_max_minus_min)];
  }

  Spectrum() {}
  Spectrum(const pupumath::vec3& linear_rgb);
  Spectrum(float v) { samples.fill(v); }
};

namespace spectrum_ns {
pupumath::vec3 spectrum_sample_to_xyz(float wavelength, float amplitude);
pupumath::vec3 xyz_to_linear_rgb(const pupumath::vec3& xyz);
pupumath::vec3 linear_rgb_to_xyz(const pupumath::vec3& rgb);
pupumath::vec3 srgb_to_xyz(pupumath::vec3 rgb);
std::array<float, Spectrum::count>
linear_rgb_to_spectrum(const pupumath::vec3& rgb);
}
