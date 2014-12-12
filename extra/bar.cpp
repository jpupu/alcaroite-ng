#pragma once

#include <cmath>

namespace pupumath_plain {

struct vec3 {
    union {
        struct { float x, y, z; };
        float data[3];
    };

    vec3 () {}
    vec3 (float x, float y, float z) : x(x), y(y), z(z) {}
    vec3 (float f) : x(f), y(f), z(f) {}
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
};

inline
vec3 operator- (const vec3& a)
{
    return vec3{ -a.x, -a.y, -a.z };
}

inline
vec3 operator+ (const vec3& a, const vec3& b)
{
    return vec3{ a.x+b.x, a.y+b.y, a.z+b.z };
}

inline
vec3 operator- (const vec3& a, const vec3& b)
{
    return vec3{ a.x-b.x, a.y-b.y, a.z-b.z };
}

inline
vec3 operator* (const vec3& a, float f)
{
    return vec3{ a.x*f, a.y*f, a.z*f };
}

inline
vec3 operator* (float f, const vec3& a) { return a * f; }

inline
vec3 operator/ (const vec3& a, float f)
{
    return vec3{ a.x/f, a.y/f, a.z/f };
}

inline
vec3 operator/ (float f, const vec3& a) { return a / f; }

inline
float dot (const vec3& a, const vec3& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline
float norm_sq (const vec3& v)
{
    return dot(v,v);
}

// inline
float norm (const vec3& v)
{
    return std::sqrt(dot(v,v));
}

inline
vec3 normalize (const vec3& v)
{
    return v / norm(v);
}

struct mat34 {
    float data[12];
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
};

inline
vec3 mul (const mat34& m, const vec3& v)
{
    return vec3{ m[0]*v[0] + m[1]*v[1] +  m[2]*v[2] + m[3],
                 m[4]*v[0] + m[5]*v[1] +  m[6]*v[2] + m[7],
                 m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11] };
}

inline
vec3 mul_rotation (const mat34& m, const vec3& v)
{
    return vec3{ m[0]*v[0] + m[1]*v[1] +  m[2]*v[2],
                 m[4]*v[0] + m[5]*v[1] +  m[6]*v[2],
                 m[8]*v[0] + m[9]*v[1] + m[10]*v[2] };
}

inline
vec3 mul_rotation_transpose (const mat34& m, const vec3& v)
{
    return vec3{ m[0]*v[0] + m[4]*v[1] +  m[8]*v[2],
                 m[1]*v[0] + m[5]*v[1] +  m[9]*v[2],
                 m[2]*v[0] + m[6]*v[1] + m[10]*v[2] };
}

// 0 1 2  3      0 1 2  3 
// 4 5 6  7   x  4 5 6  7
// 8 9 10 11     8 9 10 11
// _ _ _  I      _ _ _  I

// inline
mat34 mul (const mat34& a, const mat34& b)
{
    return mat34{
        a[0]*b[0] + a[1]*b[4] + a[2]*b[8],
        a[0]*b[1] + a[1]*b[5] + a[2]*b[9],
        a[0]*b[2] + a[1]*b[6] + a[2]*b[10],
        a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3],
        
        a[4]*b[0] + a[5]*b[4] + a[6]*b[8],
        a[4]*b[1] + a[5]*b[5] + a[6]*b[9],
        a[4]*b[2] + a[5]*b[6] + a[6]*b[10],
        a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7],

        a[8]*b[0] + a[9]*b[4] + a[10]*b[8],
        a[8]*b[1] + a[9]*b[5] + a[10]*b[9],
        a[8]*b[2] + a[9]*b[6] + a[10]*b[10],
        a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11],
     };
}

inline
mat34 operator* (const mat34& a, const mat34& b)
{
    return mul(a, b);
}

} // namespace pupumath_plain

