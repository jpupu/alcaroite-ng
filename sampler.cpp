#include "sampler.hpp"
#include "util.hpp"
using pupumath::vec3;

Sampler::Sampler(int n)
    : n(n), wavelen(new float[n]), lens(new vec3[n]), shading(new vec3[n])
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
      lens[i] = vec3{frand(), frand(), 0};
    }
  }

  {

    const float step = 1.f / n;
    for (int i = 0; i < n; i++) {
      shading[i] = vec3{(i + frand()) * step, (i + frand()) * step, 0};
      // shading[i] = vec3{ frand(), frand(), 0 };
    }
    // Shuffle u1 values -> latin hypercube.
    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(shading[i].x, shading[j].x);
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
    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() % (n - i);
      std::swap(shading[i], shading[j]);
    }
  }
}
