#include <glm/glm.hpp>  
#include <smmintrin.h>

struct vec3 {

    union {
        struct{
            float x, y, z;
        };
        float data[3];
    };
    float operator[](int i)const{return data[i];}
};

struct mat34 {
    float data[12];
    float operator[](int i)const{return data[i];}
};

vec3 mul (const mat34& m, const vec3& v)
{
    return vec3{ m[0]*v[0] + m[1]*v[1] +  m[2]*v[2] + m[3],
                 m[4]*v[0] + m[5]*v[1] +  m[6]*v[2] + m[7],
                 m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11] };
}

struct vec3a {

    union {
        struct{
    float x, y, z, w;
};
    float data[4];
        __m128 mm;
    };
    float operator[](int i)const{return data[i];}
};

struct mat34a {
    float data[16];
    float operator[](int i)const{return data[i];}
};


// vec3a cross (const float* __restrict__ x, const float* __restrict__ y)
// {
//     vec3a ret = vec3a( x[0] * (x[1]*y[2]-x[2]*y[1]),
//                        x[1] * (x[2]*y[0]-x[0]*y[2]),
//                        x[2] * (x[0]*y[1]-x[1]*y[0]));
// }

vec3 mul (const float* __restrict__ m, const float* __restrict__ v)
{
    // const float* m = m_.data;
    // const float* v = v_.data;
    __builtin_assume_aligned(m, 16);
    __builtin_assume_aligned(v, 16);
    // vec3 ret;
    // for (size_t i = 0; i < 4; i++) {
    //     ret[i] = m[i*4+0]
    // }
    vec3 ret;
    for (int i = 0; i < 4; i++) {
        ret.data[i] = m[i] * v[i];
    }
    // return vec3{ m[0]*v[0] + m[1]*v[1] +  m[2]*v[2] + m[3]*v[3],
    //              m[4]*v[0] + m[5]*v[1] +  m[6]*v[2] + m[7]*v[3],
    //              m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3] };
}

#define SIZE    (1L << 16)
void test3(double * __restrict__ a, double * __restrict__ b)
{
    int i;

    __builtin_assume_aligned(a, 16);
    __builtin_assume_aligned(b, 16);

    for (i = 0; i < SIZE; i++)
    {
        a[i] += b[i];
    }
}

void test4(double * __restrict__ a, double * __restrict__ b)
{
    int i;

    double *x = (double*)__builtin_assume_aligned(a, 16);
    double *y = (double*)__builtin_assume_aligned(b, 16);

    for (i = 0; i < SIZE; i++)
    {
        x[i] += y[i];
    }
}

void test5(float * __restrict__ a, float * __restrict__ b)
{
    int i;

    float *x = (float*)__builtin_assume_aligned(a, 16);
    float *y = (float*)__builtin_assume_aligned(b, 16);

    for (i = 0; i < SIZE; i++)
    {
        x[i] += y[i];
    }
}

void test6(float * __restrict__ a, float * __restrict__ b)
{
    int i;

    float *x = (float*)__builtin_assume_aligned(a, 16);
    float *y = (float*)__builtin_assume_aligned(b, 16);

    for (i = 0; i < 4; i++)
    {
        x[i] += y[i];
    }
}

void test7(vec3a& __restrict__ a, vec3a& __restrict__ b)
{
    int i;

    float *x = (float*)__builtin_assume_aligned(a.data, 16);
    float *y = (float*)__builtin_assume_aligned(b.data, 16);

    for (i = 0; i < 4; i++)
    {
        x[i] *= y[i];
    }
}

vec3a test8b(float * __restrict__ a, float * __restrict__ b)
{
    vec3a ret;
    int i;

    float *x = (float*)__builtin_assume_aligned(a, 16);
    float *y = (float*)__builtin_assume_aligned(b, 16);

    for (i = 0; i < 4; i++)
    {
        ret.data[i] = x[i] * y[i];
    }
    return ret;
}

vec3a test8(vec3a& __restrict__ a, vec3a& __restrict__ b)
{
    vec3a ret;
    int i;

    float *x = (float*)__builtin_assume_aligned(a.data, 16);
    float *y = (float*)__builtin_assume_aligned(b.data, 16);

    for (i = 0; i < 4; i++)
    {
        ret.data[i] = x[i] * y[i];
    }
    return ret;
}

// vec3a __test9__(mat34a& __restrict__ a, vec3a& __restrict__ b)
// {
//     vec3a ret;
//     int i;

//     float *x = (float*)__builtin_assume_aligned(a.data, 16);
//     float *y = (float*)__builtin_assume_aligned(b.data, 16);

//     // for (i = 0; i < 4; i++)
//     // {
//     //     ret.data[i] = x[i] * y[i];
//     // }
//     ret.data[0] = x[0] * y[0] +
//                   x[1] * y[1] +
//                   x[2] * y[2] +
//                   x[3] * y[3];
//     return ret;
// }

float __test9__(const float* __restrict__ a, const float* __restrict__ b)
{
    float *x = (float*)__builtin_assume_aligned(a, 16);
    float *y = (float*)__builtin_assume_aligned(b, 16);

    return  x[0] * y[0] +
                  x[1] * y[1] +
                  x[2] * y[2] +
                  x[3] * y[3];
}

float __test9_3__(const float* __restrict__ a, const float* __restrict__ b)
{
    float *x = (float*)__builtin_assume_aligned(a, 16);
    float *y = (float*)__builtin_assume_aligned(b, 16);

    return  x[0] * y[0] +
                  x[1] * y[1] +
                  x[2] * y[2];
}


// double dotprod(double *__restrict__ a, double *__restrict__ b, int n)
// {
//    __m128d aa, bb, cc, ss;
//    int i, n1 = n - 1;
//    double *s = (double*)calloc(2, sizeof(double));
//    double s2 = 0;

//    ss = _mm_set1_pd(0);

//    for(i = 0 ; i < n1 ; i += 2) 
//    {
//      aa = _mm_load_pd(a + i);
//      bb = _mm_load_pd(b + i);
//      cc = _mm_mul_pd(aa, bb);
//      ss = _mm_add_pd(ss, cc);
//    }

//    _mm_store_pd(s, ss);
//    s2 = s[0] + s[1];

//    if(i < n)
//       s2 += a[i] * b[i];

//    free(s);

//    return s2;
// }

vec3a __intrinsic_mul__(float *__restrict__ a, float *__restrict__ b)
{
   __m128 aa, bb, cc, ss;
   vec3a s;   

   aa = _mm_load_ps(a);
   bb = _mm_load_ps(b);
   cc = _mm_mul_ps(aa, bb);
   _mm_store_ps(s.data, cc);

   return s;
}

float __intrinsic_dot__(const float *__restrict__ a, const float *__restrict__ b)
{
   __m128 aa, bb, cc;
   float s[4];   
   // for (int i = 0; i < 10000; i++) a[0] *= b[0];
   aa = _mm_load_ps(a);
   bb = _mm_load_ps(b);
   cc = _mm_dp_ps(aa, bb, 0xf1);
   return _mm_cvtss_f32(cc);
   // cc = _mm_mul_ps(aa, bb);
   // _mm_store_ps(s, cc);

   // return s[0];//+s[1]+s[2]+s[3];
}

float __intrinsic_dot3__(const float *__restrict__ a, const float *__restrict__ b)
{
   __m128 aa, bb, cc;
   float s[4];   
   // for (int i = 0; i < 10000; i++) a[0] *= b[0];
   aa = _mm_load_ps(a);
   bb = _mm_load_ps(b);
   cc = _mm_dp_ps(aa, bb, 0x71);
   return _mm_cvtss_f32(cc);
   // cc = _mm_mul_ps(aa, bb);
   // _mm_store_ps(s, cc);

   // return s[0];//+s[1]+s[2]+s[3];
}

float __intrinsic_dot3_v__(const vec3a& a, const vec3a& b)
{
   __m128 aa, bb, cc;
   aa = _mm_load_ps(a.data);
   bb = _mm_load_ps(b.data);
   cc = _mm_dp_ps(aa, bb, 0x71);
   return _mm_cvtss_f32(cc);
}

float __intrinsic_dot3_vm__(const vec3a& a, const vec3a& b)
{
   __m128 aa, bb, cc;
   // aa = _mm_load_ps(a.data);
   // bb = _mm_load_ps(b.data);
   cc = _mm_dp_ps(a.mm, b.mm, 0x71);
   return _mm_cvtss_f32(cc);
}

float __glm_dot__(const float *__restrict__ a, const float *__restrict__ b)
{
    glm::vec4* x = (glm::vec4*)a;
    glm::vec4* y = (glm::vec4*)b;
    return glm::dot(*x, *y);
}

constexpr long N = 10000;
constexpr long K = 100000;

float time1 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            // c += __intrinsic_dot__(a+i*4, b+i*4);
            c += __test9__(a+j*4, b+j*4);
        }
    }
    return c;
}

float time2 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            c += __intrinsic_dot__(a+j*4, b+j*4);
        }
    }
    return c;
}

float time3 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            c += __test9_3__(a+j*4, b+j*4);
        }
    }
    return c;
}

float time4 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            c += __intrinsic_dot3__(a+j*4, b+j*4);
        }
    }
    return c;
}

float time5 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            c += __glm_dot__(a+j*4, b+j*4);
        }
    }
    return c;
}

float time6 (const float* __restrict__ a, const float* __restrict__ b)
{
    float c;
    const vec3a* aa = (const vec3a*)a;
    const vec3a* bb = (const vec3a*)b;

    for (int k = 0; k < K; k++) {
        for (long i = 0; i < N; i++) {
            int j = (i*16)%N;
            c += __intrinsic_dot3_v__(aa[j], bb[j]);
        }
    }
    return c;
}


using glm::dot;
using glm::normalize;

template<typename VEC>
bool intersect (const VEC& origin, const VEC& direction,
                float tmin, float& tmax, VEC& point, VEC& normal)
{
    float b = 2 * dot(origin, direction);
    float c = dot(origin, origin) - 1.0;
    float d = b*b - 4*c;

    if (d < 0) {
        return false;
    }

    // float t1 = (-b - sqrtf(d)) / 2.0;
    // float t2 = (-b + sqrtf(d)) / 2.0;
    float t1 = (b + sqrtf(d)) / -2.0;
    float t2 = (b - sqrtf(d)) / -2.0;

    float t = t1;
    if (t > tmax) return false;
    if (t < tmin) t = t2;
    if (t < tmin) return false;
    if (t > tmax) return false;

    point = origin + direction * t;
    normal = normalize(point);
    tmax = t;
    return true;
}


template<typename VEC, typename MAT>
float itest ()
{
    float result = 0.0;
    VEC pp(0.0,0.0,0.0);
    VEC nn(0.0,0.0,0.0);
    for (int ci = 0; ci < 100; ci++)
    for (int ca = 0; ca < 100; ca++)
    for (int y = -100; y<100; y++) {
        for (int x = -100; x < 100; x++) {
            VEC origin = VEC(x/10.0, y/10.0, 2.0);
            VEC dir = VEC(0.0,0.0,0.0) - origin;

            float tmax = 1000.0;
            VEC point;
            VEC normal;
            bool hit = intersect(origin, dir, 0.1, tmax, point, normal);
            if (hit) {
                result += tmax;
                // pp = pp + point;
                // nn = nn + normal;
            }
        }
    }
    return result + pp.x + pp.y + pp.z + nn.x + nn.y + nn.z;
}



#include "vm.hpp"
#include <cstdio>
#include <chrono>
int main (int argc, char* argv[])
{
    float a[N*4] __attribute__ ((aligned (16)));
    float b[N*4] __attribute__ ((aligned (16)));
    mat34 m;
    vec3 v;

    vec3 w = mul(m,v);

    float r;
    switch (argv[1][0]) {
        case '1': r = time1(a,b); break;
        case '2': r = time2(a,b); break;
        case '3': r = time3(a,b); break;
        case '4': r = time4(a,b); break;
        case '5': r = time5(a,b); break;
        case '6': r = time6(a,b); break;
        default: printf("error!\n");
    }
    printf("r = %d\n", r);


    // float c;

    // for (int k = 0; k < 10000; k++) {
    //     for (long i = 0; i < N; i++) {
    //         // c += __intrinsic_dot__(a+i*4, b+i*4);
    //         c += __test9__(a+i*4, b+i*4);
    //     }
    // }
    // return c;
    {
        // {
            vec3p a(1,2,3);
            vec3p b(10,20,30);
            volatile vec3p c = a + b;
            // printf("  %f %f %f\n", a.x, a.y, a.z);
            // printf("+ %f %f %f\n", b.x, b.y, b.z);
            // printf("= %f %f %f\n", c.x, c.y, c.z);
        // }
        // {
            vec3s a2(1,2,3);
            vec3s b2(10,20,30);
            volatile vec3s c2 = a2 + b2;
            // printf("  %f %f %f\n", a.x, a.y, a.z);
            // printf("+ %f %f %f\n", b.x, b.y, b.z);
            // printf("= %f %f %f\n", c.x, c.y, c.z);
        // }
        printf("= %f %f %f\n", c.x, c.y, c.z);
        printf("= %f %f %f\n", c2.x, c2.y, c2.z);
    }

    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        float r = itest<vec3p,mat34p>();
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        printf("itest<vec3p> %f  \tseconds: %f\n", r, elapsed_seconds.count());
    }
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        float r = itest<vec3s,mat34s>();
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        printf("itest<vec3s> %f  \tseconds: %f\n", r, elapsed_seconds.count());
    }
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        float r = itest<glm::vec3,mat34s>();
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        printf("itest<glm::vec3> %f  \tseconds: %f\n", r, elapsed_seconds.count());
    }

}
