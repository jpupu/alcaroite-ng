#include "skybox.hpp"
#include "spectrum.hpp"
#include "ValueBlock.hpp"
#define _USE_MATH_DEFINES
#include <cmath>

namespace skybox_ns {

struct Solid final : public Skybox {
  Spectrum radiance;
  Solid(const Spectrum &radiance) : radiance(radiance) {}
  float sample(const pupumath::vec3 &dir, float wavelen) override
  {
    return radiance.sample(wavelen);
  }
};

struct Fancy final : public Skybox {
  float sample(const pupumath::vec3 &dir, float wavelen) override
  {
    float theta = M_PI / 2 - acosf(dir.y);
    float phi = atan2f(dir.x, dir.z);

    constexpr float F = M_PI / 180;
    float L = 0.5f + theta;
    // if (phi > 350*F || phi < 30*F) L = 1.0;
    if (40 * F < phi && phi < 100 * F && theta > 40 * F && theta < 70 * F)
      L = 8.0;
    if (-110 * F < phi && phi < -30 * F && theta > 20 * F && theta < 50 * F)
      L = 6.0;
    // if (theta > 0*F) L = 1.0;

    return L;
    // return std::max(0.0f, sinf(phi*10));
    // return std::max(0.0f, dot(v, vec3(0,1,0)));
  }
};

} // namespace skybox_ns

using namespace skybox_ns;

std::shared_ptr<Skybox> build_skybox(const ValueBlock &block)
{
  auto type = block.get<std::string>("type");
  if (type == "solid") {
    return std::make_shared<Solid>(block.get<Spectrum>("radiance"));
  }
  else if (type == "fancy") {
    return std::make_shared<Fancy>();
  }
  return nullptr;
}

