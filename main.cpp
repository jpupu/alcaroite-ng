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


void basis (const vec3& normal, mat34& basis2canon, mat34& canon2basis)
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

    basis2canon = { tangent.x, bitangent.x, normal.x, 0,
                    tangent.y, bitangent.y, normal.y, 0,
                    tangent.z, bitangent.z, normal.z, 0 };
    canon2basis = { tangent.x, tangent.y, tangent.z, 0, 
                    bitangent.x, bitangent.y, bitangent.z, 0, 
                    normal.x, normal.y, normal.z, 0 };
}

#include <cstdlib>
#define M_PI 3.141592
float frand ()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

vec3 sample_hemisphere (float u1, float u2)
{
    float theta = u1 * M_PI * 2;
    float phi = u2;
    return vec3(sin(theta)*phi, cos(theta)*phi, sqrtf(1+phi*phi));
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

    mat34 to_tangent, from_tangent;
    basis(ray.normal, from_tangent, to_tangent);

    vec3 wo_t = mul_rotation(to_tangent, -ray.direction);
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




int main ()
{
    constexpr int W = 100;
    constexpr int H = 100;
    vec3 origin = vec3(0,-0,2);
    FILE* fp = fopen("foo.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", W, H);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            vec3 direction = normalize(vec3(float(x)/W * 2 - 1, -(float(y)/H * 2 - 1), -1));
            Ray ray = {origin, direction, 1000.0};
            float L = radiance(ray);
            // printf("%c", '0'+int(9*L));
            fputc(int(255*L), fp);
            fputc(int(255*L), fp);
            fputc(int(255*L), fp);
        }
        // printf("\n");
    }
    fclose(fp);
}