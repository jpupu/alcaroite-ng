#include "framebuffer.hpp"
#include "pupumath.hpp"
using namespace pupumath;

inline Pixel::Pixel() : value(vec3(0.0f)), weight(0.0001f) {}

inline vec3 Pixel::normalized() const { return value / weight; }

Framebuffer::Framebuffer(int xres, int yres)
    : xres(xres), yres(yres), pixels(new Pixel[xres * yres])
{
}

void Framebuffer::add_sample(float x, float y, const vec3 &v)
{
  pixels[int(x) + int(y) * xres].value =
      pixels[int(x) + int(y) * xres].value + v;
  pixels[int(x) + int(y) * xres].weight += 1;
}

void Framebuffer::save_ppm(const std::string& filename)
{
  auto buffer = std::make_unique<char[]>(xres * yres * 3);

  for (int i = 0; i < xres * yres; i++) {
    vec3 c = pixels[i].normalized();
    for (int k = 0; k < 3; k++) {
      float ck = c[k];
      // ck = ck / (1+ck);
      // from http://en.wikipedia.org/wiki/SRGB
      if (ck <= 0.0031308) {
        ck *= 12.92;
      }
      else {
        float a = 0.055;
        ck = (1 + a) * powf(ck, 1 / 2.4) - a; // is 2.4 right?
      }
      ck = std::min(1.0f, std::max(0.0f, ck));
      buffer[i * 3 + k] = int(255 * ck);
    }
  }
  FILE *fp = fopen(filename.c_str(), "wb");
  fprintf(fp, "P6\n%d %d\n255\n", xres, yres);
  fwrite(buffer.get(), xres * yres * 3, 1, fp);
  fclose(fp);
}
