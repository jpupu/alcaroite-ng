#pragma once

#include "pupumath.hpp"
#include <cmath>
#include <cstdlib>
#include <tuple>

inline float frand()
{
  // FIXME: use c++ rng facilities
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline pupumath::mat34 basis_from_normal(const pupumath::vec3 &normal)
{
  pupumath::vec3 tangent0;
  float ax = fabs(normal.x);
  float ay = fabs(normal.y);
  float az = fabs(normal.z);
  if (ax > ay && ax > az) {
    tangent0 = pupumath::vec3(0, 0, -1);
  }
  else {
    tangent0 = pupumath::vec3(1, 0, 0);
  }
  // if (ax > ay) {
  //     if (ax > az) {
  //         tangent0 = pupumath::vec3(0,0,-1);
  //     }
  //     else {
  //         tangent0 = pupumath::vec3(1,0,0);
  //     }
  // }
  // else {
  //     if (ay > az) {
  //         tangent0 = pupumath::vec3(1,0,0);
  //     }
  //     else {
  //         tangent0 = pupumath::vec3(1,0,0);
  //     }
  // }
  pupumath::vec3 bitangent = cross(normal, tangent0);
  pupumath::vec3 tangent = cross(bitangent, normal);

  // clang-format off
  return {{ tangent.x, bitangent.x, normal.x, 0,
            tangent.y, bitangent.y, normal.y, 0,
            tangent.z, bitangent.z, normal.z, 0 }};
  // clang-format on
}

// Sampling formulas from
// http://web.cs.wpi.edu/~emmanuel/courses/cs563/S07/talks/emmanuel_agu_mc_wk10_p2.pdf

inline std::tuple<float, float> sample_disk(float u1, float u2)
{
  float theta = 2 * M_PI * u1;
  float r = sqrtf(u2);
  return std::make_tuple(cos(theta) * r, sin(theta) * r);
}

inline pupumath::vec3 sample_hemisphere(float u1, float u2)
{
  float r = sqrtf(1.0f - u1 * u1);
  float phi = 2 * M_PI * u2;
  return pupumath::vec3(cos(phi) * r, sin(phi) * r, u1);
}

inline pupumath::vec3 sample_hemisphere_cosine(float u1, float u2)
{
  float x, y;
  std::tie(x, y) = sample_disk(u1, u2);
  float z = sqrtf(std::max(0.0f, 1.0f - x * x - y * y));
  return pupumath::vec3{x, y, z};
}

inline float cos_theta(const pupumath::vec3 &v) { return v.z; }
inline float abs_cos_theta(const pupumath::vec3 &v) { return fabs(v.z); }
