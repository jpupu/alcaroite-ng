#pragma once

#include "spectrum.hpp"
#include "pupumath_struct.hpp"
#include <memory>

class Material {
protected:
  Material()
      : refractive_index(pupumath::vec3(1.0)), absorbance(pupumath::vec3(1.0))
  {
  }

public:
  Spectrum refractive_index;
  Spectrum absorbance;

  virtual float fr(const pupumath::vec3 &wo, pupumath::vec3 &wi, float wavelen,
                   float surrounding_refractive_index, float &pdf, float u1,
                   float u2) const = 0;

  float absorb(float t, float wavelen) const;
};

class ValueBlock;

std::shared_ptr<Material> build_material(const ValueBlock &);
