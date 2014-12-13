#include "pupumath_plain.hpp"

using namespace pupumath_plain;

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




bool Shape_intersect (Ray& ray)
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
    if (ax > ay) {
        if (ax > az) {
            tangent0 = vec3(0,0,-1);
        }
        else {
            tangent0 = vec3(1,0,0);
        }
    }
    else {
        if (ay > az) {
            tangent0 = vec3(1,0,0);
        }
        else {
            tangent0 = vec3(1,0,0);
        }
    }
    vec3 bitangent = cross(normal, tangent0);
    vec3 tangent = cross(bitangent, normal);

    return { tangent.x, bitangent.x, normal.x, 0,
             tangent.y, bitangent.y, normal.y, 0,
             tangent.z, bitangent.z, normal.z, 0 };
}

#include <cstdlib>
#include <tuple>
#define M_PI 3.141592
float frand ()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
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



#include <algorithm>
#include <cstdio>
float environment (const vec3& v)
{
    return std::max(0.0f, dot(v, vec3(0,1,0)));
}

float radiance (Ray& ray)
{
    if (!Shape_intersect(ray)) return environment(ray.direction);

    mat34 from_tangent = basis_from_normal(ray.normal);

    vec3 wo_t = mul_rotation_transpose(from_tangent, -ray.direction);
    // vec3 wi_t = vec3(0, 0, 1);
    // vec3 wi_t = vec3(-wo_t.x, -wo_t.y, wo_t.z);
    vec3 wi_t = sample_hemisphere(frand(), frand());
    vec3 wi_w = mul_rotation(from_tangent, wi_t);

    return environment(wi_w);

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
    float value;
    float weight;

    Pixel () : value(0.0f), weight(0.0001f) { }

    float normalized () const  { return value / weight; }
};

#include <memory>

class Framebuffer
{
public:
    int xres, yres;
    std::unique_ptr<Pixel[]> pixels;

    Framebuffer (int xres, int yres)
    : xres(xres), yres(yres), pixels(new Pixel[xres*yres])
    { }

    void add_sample (float x, float y, float v)
    {
        pixels[int(x)+int(y)*xres].value += v;
        pixels[int(x)+int(y)*xres].weight += 1;
    }

    void save_ppm ()
    {
        FILE* fp = fopen("foo.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", xres, yres);
        for (int i = 0; i < xres*yres; i++) {
            float c = pixels[i].normalized();
            c = std::min(1.0f, std::max(0.0f, c));
            fputc(int(255*c), fp);
            fputc(int(255*c), fp);
            fputc(int(255*c), fp);
        }
        fclose(fp);
    }

};



int main ()
{
    constexpr int W = 100;
    constexpr int H = 100;
    constexpr int S = 10;
    Framebuffer framebuffer(W, H);
    vec3 origin = vec3(0,-0,2);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            for (int s = 0; s < S; s++) {
                vec3 direction = normalize(vec3(float(x)/W * 2 - 1, -(float(y)/H * 2 - 1), -1));
                Ray ray = {origin, direction, 1000.0};
                float L = radiance(ray);
                framebuffer.add_sample(x, y, L);
            }
            // printf("%c", '0'+int(9*L));
        }
        // printf("\n");
    }
    framebuffer.save_ppm();
}
