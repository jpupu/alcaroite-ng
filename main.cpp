#include "framebuffer.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sampler.hpp"
#include "shape.hpp"
#include "spectrum.hpp"
#include "pupumath.hpp"
#include "ValueBlock.hpp"
#include "util.hpp"
#include <memory>

using namespace pupumath;

#include <cstdlib>
#include <tuple>
#include <vector>
#include <array>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592
#endif



class GeometricObject {
public:
    std::shared_ptr<Shape> shape;

    std::shared_ptr<Material> mat;

    int priority;

    Transform xform; // object to world
};


#include <algorithm>
#include <cstdio>
float environment (const vec3& v)
{
    // return 3.0;
    float theta = M_PI/2 - acosf(v.y);
    float phi = atan2f(v.x, v.z);
    // printf("env %f,%f,%f -> %f / %f\n", v.x,v.y,v.z, dot(v, vec3(0,1,0)), std::max(0.0f, dot(v, vec3(0,1,0))));

    constexpr float F = M_PI / 180;
    float L = 0.5f + theta;
    // if (phi > 350*F || phi < 30*F) L = 1.0;
    if (40*F < phi && phi < 100*F && theta > 40*F && theta < 70*F) L = 8.0;
    if (-110*F < phi && phi < -30*F && theta > 20*F && theta < 50*F) L = 6.0;
    // if (theta > 0*F) L = 1.0;


    return L;
    return std::max(0.0f, sinf(phi*10));
    return std::max(0.0f, dot(v, vec3(0,1,0)));
    // return 1.0;
}


std::vector<GeometricObject> objects = {
};

void build_scene(const std::vector<ValueBlock>& blocks)
{
  std::map<std::string, std::shared_ptr<Material>> materials;
  std::map<std::string, std::shared_ptr<Shape>> shapes;
  for (const auto& block : blocks) {

    if (block.type == "material") {
      materials[block.id] = build_material(block);
    }
    else if (block.type == "shape") {
      shapes[block.id] = build_shape(block);
    }
    else if (block.type == "object") {
      auto shape = shapes.at(block.get<std::string>("shape"));
      auto material = materials.at(block.get<std::string>("material"));
      objects.push_back({shape,
                         material,
                         static_cast<int>(block.get<double>("priority")),
                         {block.get<mat34>("transform"),
                          block.get<mat34>("inverse_transform")}});
    }
  }
}

class InteriorList
{
private:
    std::vector<const GeometricObject*> list;

public:
    bool has (const GeometricObject* obj) const
    {
        const auto it = find(list.begin(), list.end(), obj);
        return it != list.end();
    }

    const GeometricObject* top ()
    {
        if (list.size() == 0) return nullptr;
        return list.back();
    }

    const GeometricObject* next_top ()
    {
        if (list.size() < 2) return nullptr;
        return list[list.size()-2];
    }

    void add (const GeometricObject* obj)
    {
        list.push_back(obj);
        std::sort(list.begin(), list.end(),
            [](const GeometricObject* a, const GeometricObject* b)->bool{return a->priority<b->priority;});
    }

    void remove (const GeometricObject* obj)
    {
        auto it = find(list.begin(), list.end(), obj);
        if (it != list.end()) {
            list.erase(it);
        }
    }

    void clear ()
    {
        list.clear();
    }

    size_t size () const {
        return list.size();
    }
};

InteriorList interior;

float radiance (Ray& ray, float wavelen, Sampler& sampler, int sample_index, int nested=0)
{
    if (nested==0) { interior.clear(); }
    if (nested>100) return 0.0f;
    bool hit = false;
    // printf("--\n");
    for (const auto& o : objects) {
        // printf("test %p, %d %d\n", &o,  ray.originator==&o, interior.has(&o));
        Ray oray = { inverse_transform_point(o.xform, ray.origin), inverse_transform_vector(o.xform, ray.direction), ray.tmax, ray.originator };
        // printf("-ray: origin %f,%f,%f, dir %f,%f,%f\n", ray.origin.x,ray.origin.y,ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z);
        // printf("oray: origin %f,%f,%f, dir %f,%f,%f\n", oray.origin.x,oray.origin.y,oray.origin.z, oray.direction.x, oray.direction.y, oray.direction.z);
        if (o.shape->intersect(oray, oray.originator==&o, interior.has(&o))) {
            ray.hit_object = &o;
            hit = true;
            ray.tmax = oray.tmax;
            ray.position = transform_point(o.xform, oray.position);
            ray.normal = normalize(transform_normal(o.xform, oray.normal));
            // printf("oray: position %f,%f,%f, normal %f,%f,%f\n", oray.position.x,oray.position.y,oray.position.z, oray.normal.x, oray.normal.y, oray.normal.z);
            // printf("-ray: position %f,%f,%f, normal %f,%f,%f\n", ray.position.x,ray.position.y,ray.position.z, ray.normal.x, ray.normal.y, ray.normal.z);
        }
    }
    // printf("%s %p, tmax %f, pos %f,%f,%f, dir %f,%f,%f\n", hit?"hit":"miss",ray.hit_object, ray.tmax, ray.position.x,ray.position.y,ray.position.z, ray.direction.x, ray.direction.y, ray.direction.z);
    if (!hit) {
        // return environment(ray.direction);
        float e = environment(ray.direction);
        // printf("e=%f\n", e);
        return e;
    }

    float absorbtion = 1.0;
    if (interior.size() > 0) {
        absorbtion = interior.top()->mat->absorb(ray.tmax, wavelen);
    }


    if (dot(ray.direction, ray.normal) < 0) {
        interior.add(ray.hit_object);
    }

    bool true_intersection = (ray.hit_object == interior.top());

    vec3 wi_w;
    float factor;

    if (true_intersection) {
        float outer_refractive_index = 1.0;
        if (interior.size() > 1) {
            outer_refractive_index = interior.next_top()->mat->refractive_index.sample(wavelen);
        }

        mat34 from_tangent = basis_from_normal(ray.normal);
        mat34 to_tangent = inverse_rotation(from_tangent);

        vec3 wo_t = mul_rotation(to_tangent, -ray.direction);
        // vec3 wi_t = vec3(0, 0, 1);
        // vec3 wi_t = vec3(-wo_t.x, -wo_t.y, wo_t.z);
        vec3 sample = sampler.shading[sample_index];
        vec3 wi_t;
        float pdf;
        // float fr = brdf_lambertian(wo_t, wi_t, pdf, sample[0], sample[1]);
        // float fr = brdf_lambertian(wo_t, wi_t, pdf, frand(), frand());
        float fr = ray.hit_object->mat->fr(wo_t, wi_t, wavelen, outer_refractive_index, pdf, frand(), frand());
        wi_w = mul_rotation(from_tangent, wi_t);
        vec3 wo_w = -ray.direction;
        // printf("wo_w  %f, %f, %f\n", wo_w.x, wo_w.y, wo_w.z);
        // printf("wo_t  %f, %f, %f\n", wo_t.x, wo_t.y, wo_t.z);
        // printf("wi_t  %f, %f, %f\n", wi_t.x, wi_t.y, wi_t.z);
        // printf("wi_w  %f, %f, %f\n", wi_w.x, wi_w.y, wi_w.z);

        // SolidXyzCurveSpectrum R = { srgb_to_xyz(vec3{0.0, 1.0, 0.0}) };
        // Spectrum R = { 400, 700, {0,1,1,0} };
        Spectrum R(vec3(1,1,1));

        factor = R.sample(wavelen) * fr * abs_cos_theta(wi_t) / pdf;
        // printf("fr %f\n", fr);
    }
    else {
        wi_w = ray.direction;
        factor = 1.0;
    }

    if (dot(wi_w, ray.normal) > 0) {
        interior.remove(ray.hit_object);
    }

    Ray next_ray = {ray.position, wi_w, 1000.0, ray.hit_object};
    float L = radiance(next_ray, wavelen, sampler, sample_index, nested+1);

    // printf("L %f\n", L);
    // printf("absorbtion %f\n", absorbtion);
    // printf("fr %f\n", fr);
    // printf("pdf %f\n", pdf);
    // printf("abs_cos_theta %f\n", abs_cos_theta(wi_t));
    // return R.sample(wavelen) * absorbtion * fr * L * abs_cos_theta(wi_t) / pdf;
    return factor * absorbtion * L;

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



#include <chrono>
int main ()
{

    // vec3 wo = normalize(vec3(-1,0,-1));
    // vec3 wi;
    // float pdf;
    // btdf_dielectric(wo, wi, pdf, 1.0, 1.5);

    // printf("wo %f %f %f (%f)\n", wo.x, wo.y, wo.z, acosf(fabs(wo.z))/M_PI*180);
    // printf("wi %f %f %f (%f)\n", wi.x, wi.y, wi.z, acosf(fabs(wi.z))/M_PI*180);

    // return 0;
    auto blocks = read_valueblock_file(std::cin);
    for (const auto& block : blocks) {
        std::cout << block; 
    }
    build_scene(blocks);

    constexpr int W = 400;
    constexpr int H = 400;
    constexpr int S = 40;
    printf("Rendering %dx%d with %d samples/pixel\n", W, H, S);
    Framebuffer framebuffer(W, H);
    vec3 origin = vec3(0,0.3,2);
    Sampler sampler = Sampler(S);
    sampler.generate();
    auto start = std::chrono::system_clock::now();
// #define SINGLE   
#ifdef SINGLE
    constexpr int x0 = 50;
    constexpr int y0 = 70;
    for (int y = y0; y < y0+1; y++) {
        for (int x = x0; x < x0+1; x++) {
#else
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
#endif
    // sampler.generate();
            sampler.next();
            for (int s = 0; s < S; s++) {
                // float wavelen = frand() * 380 + 420;
                float wavelen = sampler.wavelen[s];//frand() * 380 + 420;
                vec3 direction = normalize( vec3(float(x + sampler.lens[s].x)/W * 2 - 1,
                                                 -(float(y + sampler.lens[s].y)/H * 2 - 1),
                                                 -1) );

                Ray ray = {origin, direction, 1000.0, nullptr};
                float L = radiance(ray, wavelen, sampler, s);
                // printf("--> L=%f\n", L);
#ifdef SINGLE
                L=1000;
#endif
                vec3 rgb = spectrum_ns::xyz_to_linear_rgb(spectrum_ns::spectrum_sample_to_xyz(wavelen, L));
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
