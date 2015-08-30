#pragma once

#include "pupumath_struct.hpp"

#include <cmath>

namespace pupumath {

//// vec2 //////////////////////////////////////////////////////

inline vec2 operator-(const vec2& a) { return vec2{-a.x, -a.y}; }

inline vec2 operator+(const vec2& a, const vec2& b)
{
  return vec2{a.x + b.x, a.y + b.y};
}

inline vec2 operator-(const vec2& a, const vec2& b)
{
  return vec2{a.x - b.x, a.y - b.y};
}

inline vec2 operator*(const vec2& a, float f) { return vec2{a.x * f, a.y * f}; }

inline vec2 operator*(float f, const vec2& a) { return a * f; }

inline vec2 operator/(const vec2& a, float f) { return vec2{a.x / f, a.y / f}; }

inline vec2 operator/(float f, const vec2& a) { return vec2{f / a.x, f / a.y}; }

inline float dot(const vec2& a, const vec2& b) { return a.x * b.x + a.y * b.y; }

inline float norm_sq(const vec2& v) { return dot(v, v); }

inline float norm(const vec2& v) { return std::sqrt(dot(v, v)); }

inline vec2 normalize(const vec2& v) { return v / norm(v); }

//// vec3 //////////////////////////////////////////////////////

inline vec3 operator-(const vec3& a) { return vec3{-a.x, -a.y, -a.z}; }

inline vec3 operator+(const vec3& a, const vec3& b)
{
  return vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline vec3 operator-(const vec3& a, const vec3& b)
{
  return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline vec3 operator*(const vec3& a, float f)
{
  return vec3{a.x * f, a.y * f, a.z * f};
}

inline vec3 operator*(float f, const vec3& a) { return a * f; }

inline vec3 operator/(const vec3& a, float f)
{
  return vec3{a.x / f, a.y / f, a.z / f};
}

inline vec3 operator/(float f, const vec3& a) { return a / f; }

inline float dot(const vec3& a, const vec3& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 cross(const vec3& a, const vec3& b)
{
  return vec3{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
              a[0] * b[1] - a[1] * b[0]};
}

inline float norm_sq(const vec3& v) { return dot(v, v); }

inline float norm(const vec3& v) { return std::sqrt(dot(v, v)); }

inline vec3 normalize(const vec3& v) { return v / norm(v); }

//// vec4 //////////////////////////////////////////////////////

inline vec4 operator-(const vec4& a) { return vec4{-a.x, -a.y, -a.z, -a.w}; }

inline vec4 operator+(const vec4& a, const vec4& b)
{
  return vec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline vec4 operator-(const vec4& a, const vec4& b)
{
  return vec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

inline vec4 operator*(const vec4& a, float f)
{
  return vec4{a.x * f, a.y * f, a.z * f, a.w * f};
}

inline vec4 operator*(float f, const vec4& a) { return a * f; }

inline vec4 operator/(const vec4& a, float f)
{
  return vec4{a.x / f, a.y / f, a.z / f, a.w / f};
}

inline vec4 operator/(float f, const vec4& a)
{
  return vec4{f / a.x, f / a.y, f / a.z, f / a.w};
}

inline float dot(const vec4& a, const vec4& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float norm_sq(const vec4& v) { return dot(v, v); }

inline float norm(const vec4& v) { return std::sqrt(dot(v, v)); }

inline vec4 normalize(const vec4& v) { return v / norm(v); }

/// mat3 /////////////////////////////////////////////////
// clang-format off

inline
mat3 transpose (const mat3& m)
{
  return mat3{{
      m(0,0), m(1,0), m(2,0),
      m(0,1), m(1,1), m(2,1),
      m(0,2), m(1,2), m(2,2),
  }};
}

inline
mat3 mul (const mat3& a, const mat3& b)
{
  return mat3{{
      a(0,0)*b(0,0) + a(0,1)*b(1,0) + a(0,2)*b(2,0),
      a(0,0)*b(0,1) + a(0,1)*b(1,1) + a(0,2)*b(2,1),
      a(0,0)*b(0,2) + a(0,1)*b(1,2) + a(0,2)*b(2,2),

      a(1,0)*b(0,0) + a(1,1)*b(1,0) + a(1,2)*b(2,0),
      a(1,0)*b(0,1) + a(1,1)*b(1,1) + a(1,2)*b(2,1),
      a(1,0)*b(0,2) + a(1,1)*b(1,2) + a(1,2)*b(2,2),

      a(2,0)*b(0,0) + a(2,1)*b(1,0) + a(2,2)*b(2,0),
      a(2,0)*b(0,1) + a(2,1)*b(1,1) + a(2,2)*b(2,1),
      a(2,0)*b(0,2) + a(2,1)*b(1,2) + a(2,2)*b(2,2),
  }};
}

inline
vec3 mul (const mat3& m, const vec3 v)
{
  return vec3{
      m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2],
      m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2],
      m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2],
  };
}

inline
vec3 mul_transpose (const mat3& m, const vec3 v)
{
  return vec3{
      m(0,0)*v[0] + m(1,0)*v[1] + m(2,0)*v[2],
      m(0,1)*v[0] + m(1,1)*v[1] + m(2,1)*v[2],
      m(0,2)*v[0] + m(1,2)*v[1] + m(2,2)*v[2],
  };
}

inline
mat3 operator* (const mat3& a, const mat3& b)
{
  return mul(a, b);
}

inline
mat3 mul (const mat3& a, float b)
{
  mat3 res;
  for (int i = 0; i < 9; i++) res[i] = a[i] * b;
  return res;
}

inline
mat3 operator* (const mat3& a, float b)
{
  return mul(a, b);
}

inline
mat3 operator* (float b, const mat3& a)
{
  return mul(a, b);
}

inline
float determinant (const mat3& m)
{
  return
        m(0,0)*m(1,1)*m(2,2) + m(0,1)*m(1,2)*m(2,0)
      + m(0,2)*m(1,0)*m(2,1) - m(0,0)*m(1,2)*m(2,1)
      - m(0,1)*m(1,0)*m(2,2) - m(0,2)*m(1,1)*m(2,0);
}

inline
mat3 inverse (const mat3& m)
{
  float det = determinant(m);
  return mat3{{
      (m(1,1)*m(2,2) - m(1,2)*m(2,1))/det,
      (m(0,2)*m(2,1) - m(0,1)*m(2,2))/det,
      (m(0,1)*m(1,2) - m(0,2)*m(1,1))/det,
      (m(1,2)*m(2,0) - m(1,0)*m(2,2))/det,
      (m(0,0)*m(2,2) - m(0,2)*m(2,0))/det,
      (m(0,2)*m(1,0) - m(0,0)*m(1,2))/det,
      (m(1,0)*m(2,1) - m(1,1)*m(2,0))/det,
      (m(0,1)*m(2,0) - m(0,0)*m(2,1))/det,
      (m(0,0)*m(1,1) - m(0,1)*m(1,0))/det,
  }};
};


/// mat4 /////////////////////////////////////////////////

inline
mat4 transpose (const mat4& m)
{
  return mat4{{
      m(0,0), m(1,0), m(2,0), m(3,0),
      m(0,1), m(1,1), m(2,1), m(3,1),
      m(0,2), m(1,2), m(2,2), m(3,2),
      m(0,3), m(1,3), m(2,3), m(3,3),
  }};
}

inline
mat4 mul (const mat4& a, const mat4& b)
{
  return mat4{{
      a(0,0)*b(0,0) + a(0,1)*b(1,0) + a(0,2)*b(2,0) + a(0,3)*b(3,0),
      a(0,0)*b(0,1) + a(0,1)*b(1,1) + a(0,2)*b(2,1) + a(0,3)*b(3,1),
      a(0,0)*b(0,2) + a(0,1)*b(1,2) + a(0,2)*b(2,2) + a(0,3)*b(3,2),
      a(0,0)*b(0,3) + a(0,1)*b(1,3) + a(0,2)*b(2,3) + a(0,3)*b(3,3),

      a(1,0)*b(0,0) + a(1,1)*b(1,0) + a(1,2)*b(2,0) + a(1,3)*b(3,0),
      a(1,0)*b(0,1) + a(1,1)*b(1,1) + a(1,2)*b(2,1) + a(1,3)*b(3,1),
      a(1,0)*b(0,2) + a(1,1)*b(1,2) + a(1,2)*b(2,2) + a(1,3)*b(3,2),
      a(1,0)*b(0,3) + a(1,1)*b(1,3) + a(1,2)*b(2,3) + a(1,3)*b(3,3),

      a(2,0)*b(0,0) + a(2,1)*b(1,0) + a(2,2)*b(2,0) + a(2,3)*b(3,0),
      a(2,0)*b(0,1) + a(2,1)*b(1,1) + a(2,2)*b(2,1) + a(2,3)*b(3,1),
      a(2,0)*b(0,2) + a(2,1)*b(1,2) + a(2,2)*b(2,2) + a(2,3)*b(3,2),
      a(2,0)*b(0,3) + a(2,1)*b(1,3) + a(2,2)*b(2,3) + a(2,3)*b(3,3),

      a(3,0)*b(0,0) + a(3,1)*b(1,0) + a(3,2)*b(2,0) + a(3,3)*b(3,0),
      a(3,0)*b(0,1) + a(3,1)*b(1,1) + a(3,2)*b(2,1) + a(3,3)*b(3,1),
      a(3,0)*b(0,2) + a(3,1)*b(1,2) + a(3,2)*b(2,2) + a(3,3)*b(3,2),
      a(3,0)*b(0,3) + a(3,1)*b(1,3) + a(3,2)*b(2,3) + a(3,3)*b(3,3),
  }};
}

inline
vec4 mul (const mat4& m, const vec4 v)
{
  return vec4{
      m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3)*v[3],
      m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3)*v[3],
      m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3)*v[3],
      m(3,0)*v[0] + m(3,1)*v[1] + m(3,2)*v[2] + m(3,3)*v[3],
  };
}

inline
vec4 mul_transpose (const mat4& m, const vec4 v)
{
  return vec4{
      m(0,0)*v[0] + m(1,0)*v[1] + m(2,0)*v[2] + m(3,0)*v[3],
      m(0,1)*v[0] + m(1,1)*v[1] + m(2,1)*v[2] + m(3,1)*v[3],
      m(0,2)*v[0] + m(1,2)*v[1] + m(2,2)*v[2] + m(3,2)*v[3],
      m(0,3)*v[0] + m(1,3)*v[1] + m(2,3)*v[2] + m(3,3)*v[3],
  };
}

inline
mat4 operator* (const mat4& a, const mat4& b)
{
  return mul(a, b);
}

inline
mat4 mul (const mat4& a, float b)
{
  mat4 res;
  for (int i = 0; i < 16; i++) res[i] = a[i] * b;
  return res;
}

inline
mat4 operator* (const mat4& a, float b)
{
  return mul(a, b);
}

inline
mat4 operator* (float b, const mat4& a)
{
  return mul(a, b);
}

inline
float determinant (const mat4& m)
{
  return
        m(0,3) * m(1,2) * m(2,1) * m(3,0)
      - m(0,2) * m(1,3) * m(2,1) * m(3,0)
      - m(0,3) * m(1,1) * m(2,2) * m(3,0)
      + m(0,1) * m(1,3) * m(2,2) * m(3,0)
      + m(0,2) * m(1,1) * m(2,3) * m(3,0)
      - m(0,1) * m(1,2) * m(2,3) * m(3,0)

      - m(0,3) * m(1,2) * m(2,0) * m(3,1)
      + m(0,2) * m(1,3) * m(2,0) * m(3,1)
      + m(0,3) * m(1,0) * m(2,2) * m(3,1)
      - m(0,0) * m(1,3) * m(2,2) * m(3,1)
      - m(0,2) * m(1,0) * m(2,3) * m(3,1)
      + m(0,0) * m(1,2) * m(2,3) * m(3,1)

      + m(0,3) * m(1,1) * m(2,0) * m(3,2)
      - m(0,1) * m(1,3) * m(2,0) * m(3,2)
      - m(0,3) * m(1,0) * m(2,1) * m(3,2)
      + m(0,0) * m(1,3) * m(2,1) * m(3,2)
      + m(0,1) * m(1,0) * m(2,3) * m(3,2)
      - m(0,0) * m(1,1) * m(2,3) * m(3,2)

      - m(0,2) * m(1,1) * m(2,0) * m(3,3)
      + m(0,1) * m(1,2) * m(2,0) * m(3,3)
      + m(0,2) * m(1,0) * m(2,1) * m(3,3)
      - m(0,0) * m(1,2) * m(2,1) * m(3,3)
      - m(0,1) * m(1,0) * m(2,2) * m(3,3)
      + m(0,0) * m(1,1) * m(2,2) * m(3,3);
}

inline
mat4 inverse (const mat4& m)
{
  auto mm = mat4{{
      m(1,2)*m(2,3)*m(3,1) - m(1,3)*m(2,2)*m(3,1) + m(1,3)*m(2,1)*m(3,2) -
          m(1,1)*m(2,3)*m(3,2) - m(1,2)*m(2,1)*m(3,3) + m(1,1)*m(2,2)*m(3,3),
      m(0,3)*m(2,2)*m(3,1) - m(0,2)*m(2,3)*m(3,1) - m(0,3)*m(2,1)*m(3,2) +
          m(0,1)*m(2,3)*m(3,2) + m(0,2)*m(2,1)*m(3,3) - m(0,1)*m(2,2)*m(3,3),
      m(0,2)*m(1,3)*m(3,1) - m(0,3)*m(1,2)*m(3,1) + m(0,3)*m(1,1)*m(3,2) - m(0,1)*m(1,3)*m(3,2) - m(0,2)*m(1,1)*m(3,3) + m(0,1)*m(1,2)*m(3,3),
      m(0,3)*m(1,2)*m(2,1) - m(0,2)*m(1,3)*m(2,1) - m(0,3)*m(1,1)*m(2,2) + m(0,1)*m(1,3)*m(2,2) + m(0,2)*m(1,1)*m(2,3) - m(0,1)*m(1,2)*m(2,3),
      m(1,3)*m(2,2)*m(3,0) - m(1,2)*m(2,3)*m(3,0) - m(1,3)*m(2,0)*m(3,2) + m(1,0)*m(2,3)*m(3,2) + m(1,2)*m(2,0)*m(3,3) - m(1,0)*m(2,2)*m(3,3),
      m(0,2)*m(2,3)*m(3,0) - m(0,3)*m(2,2)*m(3,0) + m(0,3)*m(2,0)*m(3,2) - m(0,0)*m(2,3)*m(3,2) - m(0,2)*m(2,0)*m(3,3) + m(0,0)*m(2,2)*m(3,3),
      m(0,3)*m(1,2)*m(3,0) - m(0,2)*m(1,3)*m(3,0) - m(0,3)*m(1,0)*m(3,2) + m(0,0)*m(1,3)*m(3,2) + m(0,2)*m(1,0)*m(3,3) - m(0,0)*m(1,2)*m(3,3),
      m(0,2)*m(1,3)*m(2,0) - m(0,3)*m(1,2)*m(2,0) + m(0,3)*m(1,0)*m(2,2) - m(0,0)*m(1,3)*m(2,2) - m(0,2)*m(1,0)*m(2,3) + m(0,0)*m(1,2)*m(2,3),
      m(1,1)*m(2,3)*m(3,0) - m(1,3)*m(2,1)*m(3,0) + m(1,3)*m(2,0)*m(3,1) - m(1,0)*m(2,3)*m(3,1) - m(1,1)*m(2,0)*m(3,3) + m(1,0)*m(2,1)*m(3,3),
      m(0,3)*m(2,1)*m(3,0) - m(0,1)*m(2,3)*m(3,0) - m(0,3)*m(2,0)*m(3,1) + m(0,0)*m(2,3)*m(3,1) + m(0,1)*m(2,0)*m(3,3) - m(0,0)*m(2,1)*m(3,3),
      m(0,1)*m(1,3)*m(3,0) - m(0,3)*m(1,1)*m(3,0) + m(0,3)*m(1,0)*m(3,1) - m(0,0)*m(1,3)*m(3,1) - m(0,1)*m(1,0)*m(3,3) + m(0,0)*m(1,1)*m(3,3),
      m(0,3)*m(1,1)*m(2,0) - m(0,1)*m(1,3)*m(2,0) - m(0,3)*m(1,0)*m(2,1) + m(0,0)*m(1,3)*m(2,1) + m(0,1)*m(1,0)*m(2,3) - m(0,0)*m(1,1)*m(2,3),
      m(1,2)*m(2,1)*m(3,0) - m(1,1)*m(2,2)*m(3,0) - m(1,2)*m(2,0)*m(3,1) + m(1,0)*m(2,2)*m(3,1) + m(1,1)*m(2,0)*m(3,2) - m(1,0)*m(2,1)*m(3,2),
      m(0,1)*m(2,2)*m(3,0) - m(0,2)*m(2,1)*m(3,0) + m(0,2)*m(2,0)*m(3,1) - m(0,0)*m(2,2)*m(3,1) - m(0,1)*m(2,0)*m(3,2) + m(0,0)*m(2,1)*m(3,2),
      m(0,2)*m(1,1)*m(3,0) - m(0,1)*m(1,2)*m(3,0) - m(0,2)*m(1,0)*m(3,1) + m(0,0)*m(1,2)*m(3,1) + m(0,1)*m(1,0)*m(3,2) - m(0,0)*m(1,1)*m(3,2),
      m(0,1)*m(1,2)*m(2,0) - m(0,2)*m(1,1)*m(2,0) + m(0,2)*m(1,0)*m(2,1) - m(0,0)*m(1,2)*m(2,1) - m(0,1)*m(1,0)*m(2,2) + m(0,0)*m(1,1)*m(2,2),
  }};

  return mm * (1 / determinant(m));
};


/// mat34 ////////////////////////////////////////////////

inline
mat34 transpose (const mat34& m)
{
  return mat34{{
      m(0,0), m(1,0), m(2,0), 0,
      m(0,1), m(1,1), m(2,1), 0,
      m(0,2), m(1,2), m(2,2), 0,
  }};
}

inline
mat34 mul (const mat34& a, const mat34& b)
{
  return mat34{{
      a(0,0)*b(0,0) + a(0,1)*b(1,0) + a(0,2)*b(2,0),
      a(0,0)*b(0,1) + a(0,1)*b(1,1) + a(0,2)*b(2,1),
      a(0,0)*b(0,2) + a(0,1)*b(1,2) + a(0,2)*b(2,2),
      a(0,0)*b(0,3) + a(0,1)*b(1,3) + a(0,2)*b(2,3) + a(0,3),

      a(1,0)*b(0,0) + a(1,1)*b(1,0) + a(1,2)*b(2,0),
      a(1,0)*b(0,1) + a(1,1)*b(1,1) + a(1,2)*b(2,1),
      a(1,0)*b(0,2) + a(1,1)*b(1,2) + a(1,2)*b(2,2),
      a(1,0)*b(0,3) + a(1,1)*b(1,3) + a(1,2)*b(2,3) + a(1,3),

      a(2,0)*b(0,0) + a(2,1)*b(1,0) + a(2,2)*b(2,0),
      a(2,0)*b(0,1) + a(2,1)*b(1,1) + a(2,2)*b(2,1),
      a(2,0)*b(0,2) + a(2,1)*b(1,2) + a(2,2)*b(2,2),
      a(2,0)*b(0,3) + a(2,1)*b(1,3) + a(2,2)*b(2,3) + a(2,3),
  }};
}

inline
vec3 mul (const mat34& m, const vec3 v)
{
  return vec3{
      m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3),
      m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3),
      m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3),
  };
}

inline
vec3 mul_3x3 (const mat34& m, const vec3 v)
{
  return vec3{
      m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2],
      m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2],
      m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2],
  };
}

inline
vec3 mul_3x3_transpose (const mat34& m, const vec3 v)
{
  return vec3{
      m(0,0)*v[0] + m(1,0)*v[1] + m(2,0)*v[2],
      m(0,1)*v[0] + m(1,1)*v[1] + m(2,1)*v[2],
      m(0,2)*v[0] + m(1,2)*v[1] + m(2,2)*v[2],
  };
}

inline
mat34 operator* (const mat34& a, const mat34& b)
{
  return mul(a, b);
}

inline
mat34 mul (const mat34& a, float b)
{
  mat34 res;
  for (int i = 0; i < 12; i++) res[i] = a[i] * b;
  return res;
}

inline
mat34 operator* (const mat34& a, float b)
{
  return mul(a, b);
}

inline
mat34 operator* (float b, const mat34& a)
{
  return mul(a, b);
}

inline
float determinant (const mat34& m)
{
  return
      - m(0,2) * m(1,1) * m(2,0)
      + m(0,1) * m(1,2) * m(2,0)
      + m(0,2) * m(1,0) * m(2,1)
      - m(0,0) * m(1,2) * m(2,1)
      - m(0,1) * m(1,0) * m(2,2)
      + m(0,0) * m(1,1) * m(2,2);
}

inline
mat34 inverse (const mat34& m)
{
  auto mm = mat34{{
      -m(1,2)*m(2,1) + m(1,1)*m(2,2),
      + m(0,2)*m(2,1) - m(0,1)*m(2,2),
      -m(0,2)*m(1,1) + m(0,1)*m(1,2),
      m(0,3)*m(1,2)*m(2,1) - m(0,2)*m(1,3)*m(2,1) - m(0,3)*m(1,1)*m(2,2) + m(0,1)*m(1,3)*m(2,2) + m(0,2)*m(1,1)*m(2,3) - m(0,1)*m(1,2)*m(2,3),
      + m(1,2)*m(2,0) - m(1,0)*m(2,2),
      -m(0,2)*m(2,0) + m(0,0)*m(2,2),
      + m(0,2)*m(1,0) - m(0,0)*m(1,2),
      m(0,2)*m(1,3)*m(2,0) - m(0,3)*m(1,2)*m(2,0) + m(0,3)*m(1,0)*m(2,2) - m(0,0)*m(1,3)*m(2,2) - m(0,2)*m(1,0)*m(2,3) + m(0,0)*m(1,2)*m(2,3),
      -m(1,1)*m(2,0) + m(1,0)*m(2,1),
      + m(0,1)*m(2,0) - m(0,0)*m(2,1),
      -m(0,1)*m(1,0) + m(0,0)*m(1,1),
      m(0,3)*m(1,1)*m(2,0) - m(0,1)*m(1,3)*m(2,0) - m(0,3)*m(1,0)*m(2,1) + m(0,0)*m(1,3)*m(2,1) + m(0,1)*m(1,0)*m(2,3) - m(0,0)*m(1,1)*m(2,3),
  }};

  return mm * (1 / determinant(m));
};

/// Inverse of a matrix consisting of rotation and translation (no scaling)
inline
mat34 inverse_noscale (const mat34& a)
{
  // http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche53.html
  // http://graphics.stanford.edu/courses/cs248-98-fall/Final/q4.html

  return mat34{{
      a(0,0), a(1,0), a(2,0), -a(0,0)*a(0,3) - a(1,0)*a(1,3) - a(2,0)*a(2,3),
      a(0,1), a(1,1), a(2,1), -a(0,1)*a(0,3) - a(1,1)*a(1,3) - a(2,1)*a(2,3),
      a(0,2), a(1,2), a(2,2), -a(0,2)*a(0,3) - a(1,2)*a(1,3) - a(2,2)*a(2,3),
  }};
}

// clang-format on

inline vec3 transform_point(const Transform& t, const vec3& p)
{
  return mul(t.M, p);
}

inline vec3 transform_vector(const Transform& t, const vec3& p)
{
  return mul_3x3(t.M, p);
}

inline vec3 transform_normal(const Transform& t, const vec3& p)
{
  return mul_3x3_transpose(t.Minv, p);
}

inline vec3 inverse_transform_point(const Transform& t, const vec3& p)
{
  return mul(t.Minv, p);
}

inline vec3 inverse_transform_vector(const Transform& t, const vec3& p)
{
  return mul_3x3(t.Minv, p);
}

inline vec3 inverse_transform_normal(const Transform& t, const vec3& p)
{
  return mul_3x3_transpose(t.M, p);
}

inline Transform mul(const Transform& a, const Transform& b)
{
  return {a.M * b.M, b.Minv * a.Minv};
}

inline Transform operator*(const Transform& a, const Transform& b)
{
  return mul(a, b);
}

inline Transform inverse(const Transform& t) { return {t.Minv, t.M}; }

namespace new_transform {
// clang-format off
#pragma GCC diagnostic ignored "-Wunused-function"

  static Transform ident ()
  {
    return {
        {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
        }},
        {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
        }}
    };
  }

  static Transform translate (const vec3& v)
  {
    return {
        {{  1, 0, 0, v.x,
            0, 1, 0, v.y,
            0, 0, 1, v.z,
        }},
        {{  1, 0, 0, -v.x,
            0, 1, 0, -v.y,
            0, 0, 1, -v.z,
        }}
    };
  }

  static Transform scale (const vec3& v)
  {
    return {
        {{  v.x,   0,   0, 0,
              0, v.y,   0, 0,
              0,   0, v.z, 0,
        }},
        {{  1/v.x,     0,     0, 0,
                0, 1/v.y,     0, 0,
                0,     0, 1/v.z, 0,
        }}
    };
  }

  static Transform rotate_x (float angle_deg)
  {
    float sin_t = sinf(angle_deg*M_PI/180);
    float cos_t = cosf(angle_deg*M_PI/180);
    mat34 m = {{
        1,     0,      0, 0,
        0, cos_t, -sin_t, 0,
        0, sin_t,  cos_t, 0,
    }};
    return { m, transpose(m) };
  }

  static Transform rotate_y (float angle_deg)
  {
    float sin_t = sinf(angle_deg*M_PI/180);
    float cos_t = cosf(angle_deg*M_PI/180);
    mat34 m = {{
         cos_t, 0, sin_t, 0,
             0, 1,     0, 0,
        -sin_t, 0, cos_t, 0,
    }};
    return { m, transpose(m) };
  }

  static Transform rotate_z (float angle_deg)
  {
    float sin_t = sinf(angle_deg*M_PI/180);
    float cos_t = cosf(angle_deg*M_PI/180);
    mat34 m = {{
        cos_t, -sin_t, 0, 0,
        sin_t,  cos_t, 0, 0,
            0,      0, 1, 0,
    }};
    return { m, transpose(m) };
  }

  static Transform rotate (float angle_deg, const vec3& axis)
  {
    float sin_t = sinf(angle_deg*M_PI/180);
    float cos_t = cosf(angle_deg*M_PI/180);
    vec3 a = normalize(axis);
    mat34 m = {{
        a.x * a.x               + (1 - a.x * a.x) * cos_t,
        a.x * a.y * (1 - cos_t) - a.z * sin_t,
        a.x * a.z * (1 - cos_t) + a.y * sin_t,
        0,

        a.y * a.x * (1 - cos_t) + a.z * sin_t,
        a.y * a.y               + (1 - a.y * a.y) * cos_t,
        a.y * a.z * (1 - cos_t) - a.x * sin_t,
        0,

        a.z * a.x * (1 - cos_t) - a.y * sin_t,
        a.z * a.y * (1 - cos_t) + a.x * sin_t,
        a.z * a.z               + (1 - a.z * a.z) * cos_t,
        0,
    }};
    return { m, transpose(m) };
  }

  static Transform look_at (const vec3& eye, const vec3& target, const vec3& up)
  {
    vec3 dir = normalize(target - eye);
    vec3 left = normalize(cross(normalize(up), dir));
    vec3 up_ = cross(dir, left);
    mat34 cam_to_world = {{
        left.x, up_.x, dir.x, eye.x,
        left.y, up_.y, dir.y, eye.y,
        left.z, up_.z, dir.z, eye.z,
    }};
    return { cam_to_world, inverse_noscale(cam_to_world) };
  }
// clang-format on
} // namespace new_transform

} // namespace pupumath
