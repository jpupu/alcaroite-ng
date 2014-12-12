


struct vec3p {
    union {
        struct { float x, y, z; };
        float data[3];
    };

    vec3p () {}
    vec3p (float x, float y, float z) : x(x), y(y), z(z) {}
    vec3p (float f) : x(f), y(f), z(f) {}
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
};

inline
vec3p operator+ (const vec3p& a, const vec3p& b)
{
    return vec3p{ a.x+b.x, a.y+b.y, a.z+b.z };
}

inline
vec3p operator- (const vec3p& a, const vec3p& b)
{
    return vec3p{ a.x-b.x, a.y-b.y, a.z-b.z };
}

inline
vec3p operator* (const vec3p& a, float f)
{
    return vec3p{ a.x*f, a.y*f, a.z*f };
}

inline
vec3p operator* (float f, const vec3p& a) { return a * f; }

inline
vec3p operator/ (const vec3p& a, float f)
{
    return vec3p{ a.x/f, a.y/f, a.z/f };
}

inline
vec3p operator/ (float f, const vec3p& a) { return a / f; }

inline
float dot (const vec3p& a, const vec3p& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline
float norm (const vec3p& v)
{
    return sqrtf(dot(v,v));
}

inline
vec3p normalize (const vec3p& v)
{
    return v / norm(v);
}

struct mat34p {
    float data[12];
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
};

inline
vec3p operator* (const mat34p& m, const vec3p& v)
{
    return vec3p{ m[0]*v[0] + m[1]*v[1] +  m[2]*v[2] + m[3],
                 m[4]*v[0] + m[5]*v[1] +  m[6]*v[2] + m[7],
                 m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11] };
}


#include <smmintrin.h>

struct vec3s {
    union {
        struct { float x, y, z, w; };
        float data[4];
        __m128 mm;
    };

    vec3s () {}
    vec3s (float x, float y, float z) : mm(_mm_set_ps(0.0, z, y, x)) {  }
    vec3s (float f) : mm(_mm_set_ps1(f)) {}
    vec3s (__m128 v) : mm(v) {}
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
}  __attribute__ ((aligned (16)));

inline
vec3s operator+ (const vec3s& a, const vec3s& b)
{
    return vec3s(_mm_add_ps(a.mm, b.mm));
}

inline
vec3s operator- (const vec3s& a, const vec3s& b)
{
    return vec3s(_mm_sub_ps(a.mm, b.mm));
}

inline
vec3s operator* (const vec3s& a, const vec3s& b)
{
    return vec3s(_mm_mul_ps(a.mm, b.mm));
}

inline
vec3s operator/ (const vec3s& a, const vec3s& b)
{
    return vec3s(_mm_div_ps(a.mm, b.mm));
}

inline
vec3s operator* (const vec3s& a, float f)
{
    return vec3s(_mm_mul_ps(a.mm, _mm_set_ps1(f)));
}

inline
vec3s operator* (float f, const vec3s& a) { return a * f; }

inline
vec3s operator/ (const vec3s& a, float f)
{
    return vec3s(_mm_div_ps(a.mm, _mm_set_ps1(f)));
}

inline
vec3s operator/ (float f, const vec3s& a) { return a / f; }

inline
float dot (const vec3s& a, const vec3s& b)
{
    return _mm_cvtss_f32(_mm_dp_ps(a.mm, b.mm, 0x71));
}

inline
float norm (const vec3s& a)
{
    return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.mm, a.mm, 0x71)));
}

inline
vec3s normalize (const vec3s& a)
{
    __m128 inv_norm = _mm_rsqrt_ps(_mm_dp_ps(a.mm, a.mm, 0x7f));
    return vec3s(_mm_mul_ps(a.mm, inv_norm));
}

inline
vec3s normalize_accurate (const vec3s& a)
{
    __m128 norm = _mm_sqrt_ps(_mm_dp_ps(a.mm, a.mm, 0x7f));
    return vec3s(_mm_div_ps(a.mm, norm));
}


struct mat34s {
    float data[12];
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i) { return data[i]; }
};

inline
vec3s operator* (const mat34s& m, const vec3s& v)
{
    vec3s vv(v);
    vv.w = 1.0;
    __m128 row0 = _mm_load_ps(&m.data[0]);
    __m128 row1 = _mm_load_ps(&m.data[4]);
    __m128 row2 = _mm_load_ps(&m.data[8]);
    return vec3s( _mm_dp_ps(row0, vv.mm, 0xf1) +
                  _mm_dp_ps(row1, vv.mm, 0xf2) +
                  _mm_dp_ps(row2, vv.mm, 0xf3) +);
}

