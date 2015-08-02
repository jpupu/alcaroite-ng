#include "spectrum.hpp"
#include "pupumath.hpp"
#include <cmath>
using namespace pupumath;

Spectrum::Spectrum(const vec3 &linear_rgb)
    : samples(spectrum_ns::linear_rgb_to_spectrum(linear_rgb))
{
}

namespace spectrum_ns {

// Fitting functions from
// http://psgraphics.blogspot.fi/2014/11/converting-spectra-to-xyzrgb-values.html
// @article{Wyman2013xyz,
//    author  = {Chris Wyman and Peter-Pike Sloan and Peter Shirley},
//    title   = {Simple Analytic Approximations to the {CIE XYZ} Color Matching
//    Functions},
//    year    = {2013},
//    month   = {July},
//    day     = {12},
//    journal = {Journal of Computer Graphics Techniques (JCGT)},
//    volume  = {2},
//    number  = {2},
//    pages   = {1--11},
//    url     = {http://jcgt.org/published/0002/02/01/},
//    issn    = {2331-7418}
// }

// Inputs: wavelength in nanometers
float xFit_1931(float wave)
{
  float t1 = (wave - 442.0f) * ((wave < 442.0f) ? 0.0624f : 0.0374f);
  float t2 = (wave - 599.8f) * ((wave < 599.8f) ? 0.0264f : 0.0323f);
  float t3 = (wave - 501.1f) * ((wave < 501.1f) ? 0.0490f : 0.0382f);
  return 0.362f * expf(-0.5f * t1 * t1) + 1.056f * expf(-0.5f * t2 * t2) -
         0.065f * expf(-0.5f * t3 * t3);
}

float yFit_1931(float wave)
{
  float t1 = (wave - 568.8f) * ((wave < 568.8f) ? 0.0213f : 0.0247f);
  float t2 = (wave - 530.9f) * ((wave < 530.9f) ? 0.0613f : 0.0322f);
  return 0.821f * exp(-0.5f * t1 * t1) + 0.286f * expf(-0.5f * t2 * t2);
}

float zFit_1931(float wave)
{
  float t1 = (wave - 437.0f) * ((wave < 437.0f) ? 0.0845f : 0.0278f);
  float t2 = (wave - 459.0f) * ((wave < 459.0f) ? 0.0385f : 0.0725f);
  return 1.217f * exp(-0.5f * t1 * t1) + 0.681f * expf(-0.5f * t2 * t2);
}

vec3 spectrum_sample_to_xyz(float wavelength, float amplitude)
{
  return vec3{amplitude * xFit_1931(wavelength),
              amplitude * yFit_1931(wavelength),
              amplitude * zFit_1931(wavelength)};
}

vec3 xyz_to_linear_rgb(const vec3 &xyz)
{
  // clang-format off
  static constexpr mat34 M = {{
       3.2406, -1.5372, -0.4986, 0,
      -0.9689,  1.8758,  0.0415, 0,
       0.0557, -0.2040,  1.0570, 0, }};
  // clang-format on
  return mul_rotation(M, xyz);
}

vec3 linear_rgb_to_xyz(const vec3 &rgb)
{
  // clang-format off
  static constexpr mat34 M = {{
      0.4124, 0.3576, 0.1805, 0,
      0.2126, 0.7152, 0.0722, 0,
      0.0193, 0.1192, 0.9505, 0, }};
  // clang-format on
  return mul_rotation(M, rgb);
}

vec3 srgb_to_xyz(vec3 rgb)
{
  for (int k = 0; k < 3; k++) {
    rgb[k] = powf(rgb[k], 2.4);
  }
  return linear_rgb_to_xyz(rgb);
}

std::array<float, Spectrum::count> linear_rgb_to_spectrum(const vec3 &rgb)
{
  // http://www.cs.utah.edu/~bes/papers/color/
  // Smits, Brian: An RGB to Spectrum Conversion for Reflectances

  static float white_spectrum[Spectrum::count] = {1,     1, .9999, .9993, .9992,
                                                  .9998, 1, 1,     1,     1};
  static float red_spectrum[Spectrum::count] = {
      0.1012, 0.0515, 0, 0, 0, 0, 0.8325, 1.0149, 1.0149, 1.0149};
  static float yellow_spectrum[Spectrum::count] = {
      0.0001, 0, 0.1088, 0.6651, 1, 1, 0.9996, 0.9586, 0.9685, 0.9840};
  static float green_spectrum[Spectrum::count] = {
      0, 0, 0.0273, 0.7937, 1, 0.9418, 0.1719, 0, 0, 0.0025};
  static float cyan_spectrum[Spectrum::count] = {
      .9710, .9426, 1.0007, 1.0007, 1.0007, 1.0007, 0.1564, 0, 0, 0};
  static float blue_spectrum[Spectrum::count] = {
      1, 1, 0.8916, 0.3323, 0, 0, 0.0003, 0.0369, 0.0483, 0.0496};
  static float magenta_spectrum[Spectrum::count] = {
      1, 1, .9685, .2229, 0, 0.0458, 0.8369, 1, 1, 0.9959};

  std::array<float, Spectrum::count> spectrum = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

  float r = rgb[0];
  float g = rgb[1];
  float b = rgb[2];

  auto add = [&spectrum](float scl, float *src) {
    for (int i = 0; i < Spectrum::count; i++) {
      spectrum[i] += scl * src[i];
    }
  };

  if (r < g && r < b) {
    add(r, white_spectrum);
    if (g < b) {
      add(g - r, cyan_spectrum);
      add(b - g, blue_spectrum);
    }
    else {
      add(b - r, cyan_spectrum);
      add(g - b, green_spectrum);
    }
  }
  else if (g < r && g < b) {
    add(g, white_spectrum);
    if (r < b) {
      add(r - g, magenta_spectrum);
      add(b - r, blue_spectrum);
    }
    else {
      add(b - g, magenta_spectrum);
      add(r - b, red_spectrum);
    }
  }
  else { // (b < r && b < g)
    add(b, white_spectrum);
    if (r < g) {
      add(r - b, yellow_spectrum);
      add(g - r, green_spectrum);
    }
    else {
      add(g - b, yellow_spectrum);
      add(r - g, red_spectrum);
    }
  }

  return spectrum;
}

} // namespace_ns
