#pragma once

namespace pupumath {
struct vec2 {
  union {
    struct {
      float x, y;
    };
    float data[2];
  };

  vec2() {}
  vec2(float x, float y) : x(x), y(y) {}
  vec2(float f) : x(f), y(f) {}
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
};

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

struct vec4 {
  union {
    struct {
      float x, y, z, w;
    };
    float data[4];
  };

  vec4() {}
  vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
  vec4(float f) : x(f), y(f), z(f), w(f) {}
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
};

struct mat3 {
  float data[9];
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
  float operator()(int r, int c) const { return data[r*3+c]; }
  float &operator()(int r, int c) { return data[r*3+c]; }
};

struct mat4 {
  float data[16];
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
  float operator()(int r, int c) const { return data[r*4+c]; }
  float &operator()(int r, int c) { return data[r*4+c]; }
};

struct mat34 {
  float data[12];
  float operator[](int i) const { return data[i]; }
  float &operator[](int i) { return data[i]; }
  float operator()(int r, int c) const { return data[r*4+c]; }
  float &operator()(int r, int c) { return data[r*4+c]; }
};

struct Transform {
  mat34 M, Minv;
};
} // namespace pupumath
