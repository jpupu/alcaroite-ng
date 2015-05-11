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
vec3 cross (const vec3& a, const vec3& b)
{
    return vec3{
        a[1]*b[2] - a[2]*b[1],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0]
    };
}


inline
float norm_sq (const vec3& v)
{
    return dot(v,v);
}

inline
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

inline
mat34 transpose_rotation (const mat34& a)
{
    return mat34{{
        a[0], a[4], a[8], 0,
        a[1], a[5], a[9], 0,
        a[2], a[6], a[10], 0,
    }};
}

// 0 1 2  3      0 1 2  3 
// 4 5 6  7   x  4 5 6  7
// 8 9 10 11     8 9 10 11
// _ _ _  I      _ _ _  I

inline
mat34 mul (const mat34& a, const mat34& b)
{
    return mat34{{
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
     }};
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
mat34 inverse_rotation (const mat34& a)
{
    // http://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html

    mat34 minors = {{
        a[5]*a[10] - a[6]*a[9],
        a[4]*a[10] - a[6]*a[8],
        a[4]*a[9]  - a[5]*a[8],
        0,

        a[1]*a[10] - a[2]*a[9],
        a[0]*a[10] - a[2]*a[8],
        a[0]*a[9]  - a[1]*a[8],
        0,

        a[1]*a[6]  - a[2]*a[5],
        a[0]*a[6]  - a[2]*a[4],
        a[0]*a[5]  - a[1]*a[4],
        0,
    }};

    mat34 cofactors = {{
         minors[0], -minors[1],  minors[2], 0,
        -minors[4],  minors[5], -minors[6], 0,
         minors[8], -minors[9],  minors[10], 0,  
    }};

    mat34 adjugate = transpose_rotation(cofactors);

    float det = a[0]*minors[0] - a[1]*minors[1] + a[2]*minors[2];

    return adjugate * (1/det);
}

/// Inverse of a matrix consisting of rotation and translation (no scaling)
inline
mat34 inverse_noscale (const mat34& a)
{
    // http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche53.html
    // http://graphics.stanford.edu/courses/cs248-98-fall/Final/q4.html

    return mat34{{
        a[0], a[4], a[8],  -a[0]*a[3] - a[4]*a[7] - a[8]*a[11],
        a[1], a[5], a[9],  -a[1]*a[3] - a[5]*a[7] - a[9]*a[11],
        a[2], a[6], a[10], -a[2]*a[3] - a[6]*a[7] - a[10]*a[11],
    }};
}

struct Transform
{
    mat34 M;
    mat34 Minv;

    // Transform a point.
    vec3 point (const vec3& p) const
    {
        return mul(M, p);
    }

    // Transform a vector.
    vec3 vector (const vec3& v) const
    {
        return mul_rotation(M, v);
    }

    // Transform a normal.
    vec3 normal (const vec3& n) const
    {
        return mul_rotation_transpose(Minv, n);
    }

    // Transform a point with inverse transformation.
    vec3 inverse_point (const vec3& p) const
    {
        return mul(Minv, p);
    }

    // Transform a vector with inverse transformation.
    vec3 inverse_vector (const vec3& v) const
    {
        return mul_rotation(Minv, v);
    }

    // Transform a normal with inverse transformation.
    vec3 inverse_normal (const vec3& n) const
    {
        return mul_rotation_transpose(M, n);
    }

    Transform inverse () const
    {
        return { Minv, M };
    }

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
        return { m, transpose_rotation(m) };
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
        return { m, transpose_rotation(m) };
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
        return { m, transpose_rotation(m) };
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
        return { m, transpose_rotation(m) };
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

};

inline
Transform mul (const Transform& a, const Transform& b)
{
    return { a.M * b.M, b.Minv * a.Minv };
}

inline
Transform operator* (const Transform& a, const Transform& b)
{
    return mul(a, b);
}

} // namespace pupumath_plain

