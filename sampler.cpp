#include "sampler.hpp"
#include "spectrum.hpp"
#include "util.hpp"
using pupumath::vec3;
using pupumath::vec2;

struct LibCRandomSampler : public Sampler {

  LibCRandomSampler(int n) : Sampler(n) {}

  void generate() {}

  float get_wavelen(int sample_id) override
  {
    return Spectrum::wavelen(frand());
  }

  vec2 get_lens(int sample_id) override { return vec2{frand(), frand()}; }

  vec2 get_shading(int sample_id, int counter) override
  {
    return vec2{frand(), frand()};
  }
};

struct LhsSampler : public Sampler {
  std::unique_ptr<float[]> wavelen;
  std::unique_ptr<pupumath::vec2[]> lens;
  std::unique_ptr<pupumath::vec2[]> shading;

  LhsSampler(int n)
      : Sampler(n), wavelen(new float[n]), lens(new vec2[n]),
        shading(new vec2[n * 4])
  {
  }

  void generate() override
  {
    for (int i = 0; i < n; i++) {
      wavelen[i] = Spectrum::wavelen((i + frand()) / n);
    }

    for (int i = 0; i < n; i++) {
      lens[i] = vec2{frand(), frand()};
    }

    const float step = 1.f / n;
    for (int k = 0; k < 4; k++) {
      for (int i = 0; i < n; i++) {
        shading[k * n + i] = vec2{(i + frand()) * step, (i + frand()) * step};
      }
    }
    // Shuffle u1 values -> latin hypercube.
    for (int k = 0; k < 4; k++) {
      for (int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);
        std::swap(shading[k * n + i].x, shading[k * n + j].x);
      }
    }


    // Shuffle samples so the different dimensions are not dependent.

    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(wavelen[i], wavelen[j]);
    }

    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(lens[i], lens[j]);
    }

    for (int k = 0; k < 4; k++) {
      for (int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);
        std::swap(shading[k * n + i], shading[k * n + j]);
      }
    }
  }

  float get_wavelen(int sample_id) override { return wavelen[sample_id]; }

  vec2 get_lens(int sample_id) override { return lens[sample_id]; }

  vec2 get_shading(int sample_id, int counter) override
  {
    if (counter < 4) {
      return shading[counter * n + sample_id];
    }
    return vec2{frand(), frand()};
  }
};

std::shared_ptr<Sampler> create_sampler(int n, const std::string& name)
{
  if (name == "libcrandom")
    return std::make_shared<LibCRandomSampler>(n);
  else if (name == "lhs")
    return std::make_shared<LhsSampler>(n);
  else
    throw std::runtime_error("unknown sampler '" + name + "'");
}
