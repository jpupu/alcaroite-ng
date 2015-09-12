#include "sampler.hpp"
#include "util.hpp"
using pupumath::vec3;
using pupumath::vec2;

Sampler::Sampler(int n)
    : n(n), wavelen(new float[n]), lens(new vec2[n]), shading(new vec2[n * 4])
{
}

void Sampler::generate()
{
  {
    const float step = 340.f / n;
    for (int i = 0; i < n; i++) {
      wavelen[i] = 380.f + (i + frand()) * step;
    }
  }

  {
    for (int i = 0; i < n; i++) {
      lens[i] = vec2{frand(), frand()};
    }
  }

  {

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
  }
}

void Sampler::next()
{
  {
    // Shuffle.
    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(wavelen[i], wavelen[j]);
    }
  }

  {
    // Shuffle.
    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(lens[i], lens[j]);
    }
  }

  {
    // Shuffle samples.
    for (int k = 0; k < 4; k++) {
      for (int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);
        std::swap(shading[k * n + i], shading[k * n + j]);
      }
    }
  }
}

class RandomSampler : public Sampler {
public:
  RandomSampler(int n) : Sampler(n) {}

  void get_shading(int index, float& u1, float& u2)
  {
    u1 = frand();
    u2 = frand();
  }
};

class CleverSampler : public Sampler {
public:
  CleverSampler(int n) : Sampler(n) {}

  void get_shading(int index, float& u1, float& u2)
  {
    u1 = shading[index].x;
    u2 = shading[index].y;
  }
};

std::shared_ptr<Sampler> create_sampler(int n, const std::string& name)
{
  if (name == "random")
    return std::make_shared<RandomSampler>(n);
  else
    return std::make_shared<CleverSampler>(n);
}
