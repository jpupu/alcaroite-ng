#pragma once

namespace pupumath {
struct vec3 {
  union {
    struct {
      float x, y, z;
    };
    float data[3];
  };

  vec3() {}
  vec3(float x, float y, float z) : x(x), y(y), z(z) {}
  vec3(float f) : x(f), y(f), z(f) {}
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
};

struct mat34 {
  float data[12];
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
};

struct Transform {
  mat34 M, Minv;
};
} // namespace pupumath
