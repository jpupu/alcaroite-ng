#include "pupumath_plain.hpp"
#include <memory>

using namespace pupumath_plain;

#include <cstdlib>
#include <tuple>
#include <vector>

#ifndef M_PI
#define M_PI 3.141592
#endif

float frand ()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

struct Sampler
{
    int n;
    std::unique_ptr<float[]> wavelen;
    std::unique_ptr<vec3[]> lens;
    std::unique_ptr<vec3[]> shading;

    Sampler (int n);
    void generate ();
    void next ();
};

Sampler::Sampler (int n)
    : n(n),
    wavelen(new float[n]),
    lens(new vec3[n]),
    shading(new vec3[n])
{ }

void Sampler::generate ()
{
    {
        const float step = 380.f / n;
        for (int i = 0; i < n; i++) {
            wavelen[i] = 420.f + (i + frand()) * step;
        }
    }

    {
        for (int i = 0; i < n; i++) {
            lens[i] = vec3{ frand(), frand(), 0 };
        }
    }

    {

        const float step = 1.f / n;
        for (int i = 0; i < n; i++) {
            shading[i] = vec3{ (i + frand()) * step, (i + frand()) * step, 0 };
            // shading[i] = vec3{ frand(), frand(), 0 };
        }
        // Shuffle u1 values -> latin hypercube.
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand()%(n-i);
            std::swap(shading[i].x, shading[j].x);
        }
    }
}

void Sampler::next ()
{
    {
        // Shuffle.
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand()%(n-i);
            std::swap(wavelen[i], wavelen[j]);
        }
    }

    {
        // Shuffle.
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand()%(n-i);
            std::swap(lens[i], lens[j]);
        }
    }

    {
        // Shuffle samples.
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand()%(n-i);
            std::swap(shading[i], shading[j]);
        }
    }
}


// Fitting functions from http://psgraphics.blogspot.fi/2014/11/converting-spectra-to-xyzrgb-values.html
// @article{Wyman2013xyz,
//    author  = {Chris Wyman and Peter-Pike Sloan and Peter Shirley},
//    title   = {Simple Analytic Approximations to the {CIE XYZ} Color Matching Functions},  
//    year    = {2013},
//    month   = {July},
//    day     = {12},
//    journal = {Journal of Computer Graphics Techniques (JCGT)},
//    volume  = {2},
//    number  = {2},
//    pages   = {1--11},
//    url     = {http://jcgt.org/published/0002/02/01/},
//    issn    = {2331-7418}
// }          

// Inputs: wavelength in nanometers
float xFit_1931(float wave) {
    float t1 = (wave-442.0f) * ((wave<442.0f)?0.0624f:0.0374f);
    float t2 = (wave-599.8f) * ((wave<599.8f)?0.0264f:0.0323f);
    float t3 = (wave-501.1f) * ((wave<501.1f)?0.0490f:0.0382f);
    return 0.362f * expf(-0.5f * t1 * t1) +
           1.056f * expf(-0.5f * t2 * t2) -
           0.065f * expf(-0.5f * t3 * t3);
}
float yFit_1931(float wave) {
    float t1 = (wave-568.8f) * ((wave<568.8f)?0.0213f:0.0247f);
    float t2 = (wave-530.9f) * ((wave<530.9f)?0.0613f:0.0322f);
    return 0.821f * exp(-0.5f * t1 * t1) +
           0.286f * expf(-0.5f * t2 * t2);
}
float zFit_1931(float wave) {
    float t1 = (wave-437.0f) * ((wave<437.0f)?0.0845f:0.0278f);
    float t2 = (wave-459.0f) * ((wave<459.0f)?0.0385f:0.0725f);
    return 1.217f * exp(-0.5f * t1 * t1) +
           0.681f * expf(-0.5f * t2 * t2);
}

vec3 spectrum_sample_to_xyz (float wavelength, float amplitude)
{
    return vec3{ amplitude * xFit_1931(wavelength),
                 amplitude * yFit_1931(wavelength),
                 amplitude * zFit_1931(wavelength) };
}

vec3 xyz_to_linear_rgb (const vec3& xyz)
{
    static constexpr mat34 M = {
        3.2406, -1.5372, -0.4986, 0,
        -0.9689, 1.8758, 0.0415,  0,
        0.0557, -0.2040, 1.0570,  0,
    };
    return mul_rotation(M, xyz);
}

vec3 linear_rgb_to_xyz (const vec3& rgb)
{
    static constexpr mat34 M = {
        0.4124, 0.3576, 0.1805, 0,
        0.2126, 0.7152, 0.0722,  0,
        0.0193, 0.1192, 0.9505,  0,
    };
    return mul_rotation(M, rgb);
}

vec3 srgb_to_xyz (vec3 rgb)
{
    for (int k = 0; k < 3; k++) {
        rgb[k] = powf(rgb[k], 2.4);
    }
    return linear_rgb_to_xyz(rgb);
}

std::vector<float> linear_rgb_to_spectrum (const vec3& rgb)
{
    // http://www.cs.utah.edu/~bes/papers/color/
    // Smits, Brian: An RGB to Spectrum Conversion for Reflectances
    
    constexpr int N = 10;
    static float white_spectrum[N] = {1,1,.9999,.9993,.9992,.9998,1,1,1,1};
    static float red_spectrum[N] = {0.1012,0.0515,0,0,0,0,0.8325,1.0149,1.0149,1.0149};
    static float yellow_spectrum[N] = {0.0001,0,0.1088,0.6651,1,1,0.9996,0.9586,0.9685,0.9840};
    static float green_spectrum[N] = {0,0,0.0273,0.7937,1,0.9418,0.1719,0,0,0.0025};
    static float cyan_spectrum[N] = {.9710,.9426,1.0007,1.0007,1.0007,1.0007,0.1564,0,0,0};
    static float blue_spectrum[N] = {1,1,0.8916,0.3323,0,0,0.0003,0.0369,0.0483,0.0496};
    static float magenta_spectrum[N] = {1,1,.9685,.2229,0,0.0458,0.8369,1,1,0.9959};

    std::vector<float> spectrum(N, 0.0f);

    float r = rgb[0];
    float g = rgb[1];
    float b = rgb[2];

    auto add = [N,&spectrum](float scl, float* src) {
        for (int i = 0; i < N; i++) { spectrum[i] += scl * src[i]; }
    };

    if (r < g && r < b) {
        add(r, white_spectrum);
        if (g < b) {
            add(g-r, cyan_spectrum);
            add(b-g, blue_spectrum);
        }
        else {
            add(b-r, cyan_spectrum);
            add(g-b, green_spectrum);
        }
    }
    else if (g < r && g < b) {
        add(g, white_spectrum);
        if (r < b) {
            add(r-g, magenta_spectrum);
            add(b-r, blue_spectrum);
        }
        else {
            add(b-g, magenta_spectrum);
            add(r-b, red_spectrum);
        }
    }
    else { // (b < r && b < g)
        add(b, white_spectrum);
        if (r < g) {
            add(r-b, yellow_spectrum);
            add(g-r, green_spectrum);
        }
        else {
            add(g-b, yellow_spectrum);
            add(r-g, red_spectrum);
        }
    }

    return spectrum;
}


struct SolidSampledSpectrum
{
    float min, max;
    std::vector<float> samples;
    float sample (float wavelen)
    {
        if (wavelen < min || wavelen >= max) return 0.0f;
        return samples[int(samples.size() * (wavelen - min) / (max - min))];
    }

    SolidSampledSpectrum () {}
    SolidSampledSpectrum (const vec3& linear_rgb)
        : min(380), max(720),
        samples( linear_rgb_to_spectrum(linear_rgb) )
        { }
};

struct Ray
{
    const vec3 origin;
    const vec3 direction;

    // constraints:
    float tmax;
    // Object* originator; // the object that the ray originates from
    // int originator_subid;

    // intersection:
    vec3 position;
    vec3 normal;
    // vec3 texcoord;
    // Material* material;
    // Object* hit_object;
    // int hit_subid;

    // context: [wrong place?]
    // stack<Object*> media;
};


class Shape {
public:
    virtual bool intersect (Ray& ray) const = 0;
};

class Sphere : public Shape {
public:
    bool intersect (Ray& ray) const
    {
        float b = 2 * dot(ray.origin, ray.direction);
        float c = dot(ray.origin, ray.origin) - 1.0;
        float d = b*b - 4*c;

        if (d < 0) {
            return false;
        }

        // float t1 = (-b - sqrtf(d)) / 2.0;
        // float t2 = (-b + sqrtf(d)) / 2.0;
        float t1 = (b + sqrtf(d)) / -2.0;
        float t2 = (b - sqrtf(d)) / -2.0;

        float t = t1;
        if (t > ray.tmax) return false;
        if (t < 0.0f) t = t2;
        if (t < 0.0f) return false;
        if (t > ray.tmax) return false;

        ray.tmax = t;
        ray.position = ray.origin + ray.direction * t;
        ray.normal = normalize(ray.position);

        return true;
    }
};

class Plane : public Shape {
public:
    bool intersect (Ray& ray) const
    {
        if (ray.direction.y == 0) return false;

        float t = -ray.origin.y / ray.direction.y;

        if (t < 0.0f) return false;
        if (t > ray.tmax) return false; 

        ray.tmax = t;
        ray.position = ray.origin + ray.direction * t;
        ray.normal = vec3(0,1,0);

        return true;
    }
};


// bool Shape::intersect (Ray& ray, bool is_originator, bool inside_originator)
// {
//     // ...find t...
//     if (t >= ray.tmax) return false;

//     bool entering = dot(normal, ray.direction) < 0;

//     if (is_originator && ((inside_originator && entering) ||
//                           (!inside_originator && !entering))) {
//         return false;
//     }

//     ray.tmax = t;
//     ray.position = ray.origin + ray.direction * t;
//     ray.normal = ...;
//     ray.texcoord = ...;

//     ray.hit_subid = ...;
// }

// bool Object::intersect (Ray& ray)
// {
//     bool hit = shape->intersect(ray, this==ray.originator, this==ray.media.top());
//     if (hit) {
//         ray.hit_object = this;
//         ray.material = this->material;
//     }
//     return hit;
// }


mat34 basis_from_normal (const vec3& normal)
{
    vec3 tangent0;
    float ax = fabs(normal.x);
    float ay = fabs(normal.y);
    float az = fabs(normal.z);
    if (ax > ay && ax > az) {
        tangent0 = vec3(0,0,-1);
    }
    else {
        tangent0 = vec3(1,0,0);
    }
    // if (ax > ay) {
    //     if (ax > az) {
    //         tangent0 = vec3(0,0,-1);
    //     }
    //     else {
    //         tangent0 = vec3(1,0,0);
    //     }
    // }
    // else {
    //     if (ay > az) {
    //         tangent0 = vec3(1,0,0);
    //     }
    //     else {
    //         tangent0 = vec3(1,0,0);
    //     }
    // }
    vec3 bitangent = cross(normal, tangent0);
    vec3 tangent = cross(bitangent, normal);

    return { tangent.x, bitangent.x, normal.x, 0,
             tangent.y, bitangent.y, normal.y, 0,
             tangent.z, bitangent.z, normal.z, 0 };
}


// Sampling formulas from http://web.cs.wpi.edu/~emmanuel/courses/cs563/S07/talks/emmanuel_agu_mc_wk10_p2.pdf

std::tuple<float, float> sample_disk (float u1, float u2)
{
    float theta = 2 * M_PI * u1;
    float r = sqrtf(u2);
    return std::make_tuple(cos(theta) * r, sin(theta) * r);
}

vec3 sample_hemisphere (float u1, float u2)
{
    float r = sqrtf(1.0f - u1*u1);
    float phi = 2 * M_PI * u2;
    return vec3(cos(phi) * r, sin(phi) * r, u1);
}

vec3 sample_hemisphere_cosine (float u1, float u2)
{
    float x, y;
    std::tie(x, y) = sample_disk(u1, u2);
    float z = sqrtf( std::max(0.0f, 1.0f - x*x - y*y) );
    return vec3{x,y,z};
}

float cos_theta (const vec3& v) { return v.z; }

#include <algorithm>
#include <cstdio>
float environment (const vec3& v)
{
    return std::max(0.0f, dot(v, vec3(0,1,0)));
    // return 1.0;
}


float brdf_lambertian (const vec3& wo, vec3& wi, float& pdf, float u1, float u2)
{
    // // Cosine-weigted sampling.
    // wi = sample_hemisphere_cosine(u1, u2);
    // pdf = cos_theta(wi) / M_PI;

    // Uniform sampling.
    wi = sample_hemisphere(u1, u2);
    pdf = 1 / (2*M_PI);

    return 1.0 / M_PI;;
}

float brdf_perfect_specular (const vec3& wo, vec3& wi, float& pdf, float u1, float u2)
{
    wi = vec3{-wo.x, -wo.y, wo.z};
    pdf = 1.0;
    return 1.0 / cos_theta(wi);
}

float radiance (Ray& ray, float wavelen, Sampler& sampler, int sample_index, int nested=0)
{
    if (nested>10) return 0.0f;
    Sphere sh1 = Sphere();
    Plane sh2 = Plane();
    bool hit = sh1.intersect(ray) || sh2.intersect(ray);
    // bool hit = sh2.intersect(ray);
    if (!hit) return environment(ray.direction);

    mat34 from_tangent = basis_from_normal(ray.normal);

    vec3 wo_t = mul_rotation_transpose(from_tangent, -ray.direction);
    // vec3 wi_t = vec3(0, 0, 1);
    // vec3 wi_t = vec3(-wo_t.x, -wo_t.y, wo_t.z);
    vec3 sample = sampler.shading[sample_index];
    vec3 wi_t;
    float pdf;
    // float fr = brdf_lambertian(wo_t, wi_t, pdf, sample[0], sample[1]);
    float fr = brdf_lambertian(wo_t, wi_t, pdf, frand(), frand());
    vec3 wi_w = mul_rotation(from_tangent, wi_t);

    // SolidXyzCurveSpectrum R = { srgb_to_xyz(vec3{0.0, 1.0, 0.0}) };
    // SolidSampledSpectrum R = { 400, 700, {0,1,1,0} };
    SolidSampledSpectrum R(vec3(1,1,1));

    Ray next_ray = {ray.position, wi_w, 1000.0};
    float L = radiance(next_ray, wavelen, sampler, sample_index, nested+1);

    return R.sample(wavelen) * fr * L * cos_theta(wi_t) / pdf;

    // brdf = ray.material.brdf(ray.position);
    // wo_tangent = transform(-ray.direction);
    // def wi_tangent;
    // def pdf;
    // f,wi_tangent,pdf = brdf.evaluate(wo_tangent, ray.normal)
    // wi_world = transform(wi_tangent)
    // // update media etc...
    // return f * pdf * radiance(new_ray(ray, wi_world));

}

// float radiance (Ray& ray)
// {
//     if (!obj->intersect(ray)) return environment(ray.direction);

//     brdf = ray.material.brdf(ray.position);
//     wo_tangent = transform(-ray.direction);
//     def wi_tangent;
//     def pdf;
//     f,wi_tangent,pdf = brdf.evaluate(wo_tangent, ray.normal)
//     wi_world = transform(wi_tangent)
//     // update media etc...
//     return f * pdf * radiance(new_ray(ray, wi_world));

// }



/*

out & exiting
\   /
 \|/
--x---  fail
. . . .

in & exiting
\
 \|
--x----
. .\. .
----x--  ok
    |\

in & entering
\
 \|
--x---  fail
. .\. .

\ . . .
.\. . .
--x---  fail
  |\

out & entering
\ . . .
.\. . .
--x----
  |\|
----x--  ok
. . .\.

*/  


struct Pixel
{
    vec3 value;
    float weight;

    Pixel () : value(vec3(0.0f)), weight(0.0001f) { }

    vec3 normalized () const  { return value / weight; }
};


class Framebuffer
{
public:
    int xres, yres;
    std::unique_ptr<Pixel[]> pixels;

    Framebuffer (int xres, int yres)
    : xres(xres), yres(yres), pixels(new Pixel[xres*yres])
    { }

    void add_sample (float x, float y, const vec3& v)
    {
        pixels[int(x)+int(y)*xres].value = pixels[int(x)+int(y)*xres].value + v;
        pixels[int(x)+int(y)*xres].weight += 1;
    }

    void save_ppm ()
    {
        FILE* fp = fopen("foo.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", xres, yres);
        for (int i = 0; i < xres*yres; i++) {
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
                    ck = (1 + a) * powf(ck, 1/2.4) - a; // is 2.4 right?
                }
                ck = std::min(1.0f, std::max(0.0f, ck));
                fputc(int(255*ck), fp);
            }
        }
        fclose(fp);
    }

};


#include <chrono>
int main ()
{
    constexpr int W = 100;
    constexpr int H = 100;
    constexpr int S = 1000;
    printf("Rendering %dx%d with %d samples/pixel\n", W, H, S);
    Framebuffer framebuffer(W, H);
    vec3 origin = vec3(0,.3,2);
    Sampler sampler = Sampler(S);
    sampler.generate();
    auto start = std::chrono::system_clock::now();
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
    // sampler.generate();
            sampler.next();
            for (int s = 0; s < S; s++) {
                // float wavelen = frand() * 380 + 420;
                float wavelen = sampler.wavelen[s];//frand() * 380 + 420;
                vec3 direction = normalize( vec3(float(x + sampler.lens[s].x)/W * 2 - 1,
                                                 -(float(y + sampler.lens[s].y)/H * 2 - 1),
                                                 -1) );

                Ray ray = {origin, direction, 1000.0};
                float L = radiance(ray, wavelen, sampler, s);
                vec3 rgb = xyz_to_linear_rgb(spectrum_sample_to_xyz(wavelen, L));
                framebuffer.add_sample(x, y, rgb);
            }
            // printf("%c", '0'+int(9*L));
        }
        // printf("\n");
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    printf("Rendered in %.1f seconds\n", elapsed.count());
    framebuffer.save_ppm();
}
