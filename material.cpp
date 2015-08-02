#include "material.hpp"
#include "pupumath.hpp"
#include "util.hpp"
#include "ValueBlock.hpp"
using namespace pupumath;

float Material::absorb(float t, float wavelen) const
{
  float a = absorbance.sample(wavelen);
  return exp(-t * a);
}

float brdf_lambertian(const vec3 &wo, vec3 &wi, float &pdf, float u1, float u2)
{
  // // Cosine-weigted sampling.
  // wi = sample_hemisphere_cosine(u1, u2);
  // pdf = cos_theta(wi) / M_PI;

  // Uniform sampling.
  wi = sample_hemisphere(u1, u2);
  pdf = 1 / (2 * M_PI);

  return 1.0 / M_PI;
  ;
}

float brdf_perfect_specular_reflection(const vec3 &wo, vec3 &wi, float &pdf,
                                       float u1, float u2)
{
  wi = vec3{-wo.x, -wo.y, wo.z};
  pdf = 1.0;
  return 1.0 / abs_cos_theta(wi);
}

float brdf_transparent(const vec3 &wo, vec3 &wi, float &pdf)
{
  wi = -wo;
  pdf = 1.0;
  return 1.0 / abs_cos_theta(wi);
}

float bxdf_dielectric(const vec3 &wo, vec3 &wi, float &pdf, float n1, float n2)
{
  // Assume that the ray always comes from n1 to n2, regardless of normal
  // direction.
  bool flip = (wo.z < 0);
  pdf = 1.0;

  if (fabs(wo.z) == 1) {
    wi = -wo;
    return 1;
  }

  // printf("cos theta == %f\n", abs_cos_theta(wo));

  float cos_theta1 = abs_cos_theta(wo);
  float sin_theta1 =
      sqrtf(1 - cos_theta1 * cos_theta1); // assume 0 < cos_theta1 < pi

  float sin_theta2 = sin_theta1 * n1 / n2;

  if (sin_theta2 > 1) {
    // Total internal reflection.
    wi = vec3{-wo.x, -wo.y, wo.z};
    pdf = 1.0;
    return 1.0 / abs_cos_theta(wi);
  }

  float cos_theta2 = sqrtf(1 - sin_theta2 * sin_theta2);

  // printf("cos_theta1 %f\n", cos_theta1);
  // printf("sin_theta1 %f\n", sin_theta1);
  // printf("sin_theta2 %f\n", sin_theta2);
  // printf("cos_theta2 %f\n", cos_theta2);

  // reflectance for s-polarized light
  float Rs =
      (n1 * cos_theta1 - n2 * cos_theta2) / (n1 * cos_theta1 + n2 * cos_theta2);
  Rs = Rs * Rs;
  // reflectance for p-polarized light
  float Rp =
      (n1 * cos_theta2 - n2 * cos_theta1) / (n1 * cos_theta2 + n2 * cos_theta1);
  Rp = Rp * Rp;
  // unpolarized reflectance
  float R = (Rs + Rp) / 2;

  // printf("Rs %f  Rp %f\n", Rs, Rp);

  if (frand() < .5) {
    // printf("reflection %f\n", R);
    wi = vec3{-wo.x, -wo.y, wo.z};
    pdf = .5;
    return R / abs_cos_theta(wi);
  }
  else {
    // printf("transmission %f\n", 1-R);
    wi.x = -wo.x / sin_theta1 * sin_theta2;
    wi.y = -wo.y / sin_theta1 * sin_theta2;
    wi.z = flip ? cos_theta2 : -cos_theta2;
    pdf = .5;
    return (1 - R) / abs_cos_theta(wi);
  }

  // return 1.0 / abs_cos_theta(wi);
}

class Matte : public Material {
public:
  Matte(const Spectrum &reflectance) : reflectance(reflectance) {}

  Spectrum reflectance;

  float fr(const vec3 &wo, vec3 &wi, float wavelen,
           float surrounding_refractive_index, float &pdf, float u1,
           float u2) const
  {
    return reflectance.sample(wavelen) * brdf_lambertian(wo, wi, pdf, u1, u2);
  }
};

class PerfectMirror : public Material {
public:
  PerfectMirror(const Spectrum &reflectance) : reflectance(reflectance) {}

  Spectrum reflectance;

  float fr(const vec3 &wo, vec3 &wi, float wavelen,
           float surrounding_refractive_index, float &pdf, float u1,
           float u2) const
  {
    return reflectance.sample(wavelen) *
           brdf_perfect_specular_reflection(wo, wi, pdf, u1, u2);
  }
};

class Glass : public Material {
public:
  Glass(const Spectrum &refractive_index,
        const Spectrum &absorbance = vec3(0.0))
  {
    this->refractive_index = refractive_index;
    this->absorbance = absorbance;
  }

  float fr(const vec3 &wo, vec3 &wi, float wavelen,
           float surrounding_refractive_index, float &pdf, float u1,
           float u2) const
  {
    float n1 = surrounding_refractive_index;
    float n2 = refractive_index.sample(wavelen);
    if (wo.z < 0) {
      std::swap(n1, n2);
    }
    return bxdf_dielectric(wo, wi, pdf, n1, n2);
  }
};

class Translucent : public Material {
public:
  Translucent()
  {
    this->refractive_index = Spectrum(vec3(1.0));
    this->absorbance = vec3(0.0);
  }
  Translucent(const Spectrum &absorbance)
  {
    this->refractive_index = Spectrum(vec3(1.0));
    this->absorbance = absorbance;
  }

  float fr(const vec3 &wo, vec3 &wi, float wavelen,
           float surrounding_refractive_index, float &pdf, float u1,
           float u2) const
  {
    wi = -wo;
    pdf = 1.0;
    return 1.0 / abs_cos_theta(wi);
  }
};

std::shared_ptr<Material> build_material(const ValueBlock &block)
{
  auto type = block.get<std::string>("type");
  if (type == "glass") {
    return std::make_shared<Glass>(vec3(block.get<double>("ior")),
                                   block.get<vec3>("absorbance"));
  }
  else if (type == "matte") {
    return std::make_shared<Matte>(vec3(block.get<double>("reflectance")));
  }
  else {
    throw std::runtime_error("unknown material");
  }
}
