#include "integrator.hpp"
#include "material.hpp"
#include "pupumath.hpp"
#include "ray.hpp"
#include "sampler.hpp"
#include "scene.hpp"
#include "shape.hpp"
#include "skybox.hpp"
#include "util.hpp"
#include <algorithm>
using namespace pupumath;

/*
 * Cases of interior state and surface interface:

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

class InteriorList {
private:
  std::vector<const GeometricObject *> list;

public:
  bool has(const GeometricObject *obj) const
  {
    const auto it = find(list.begin(), list.end(), obj);
    return it != list.end();
  }

  const GeometricObject *top()
  {
    if (list.size() == 0) return nullptr;
    return list.back();
  }

  const GeometricObject *next_top()
  {
    if (list.size() < 2) return nullptr;
    return list[list.size() - 2];
  }

  void add(const GeometricObject *obj)
  {
    list.push_back(obj);
    std::sort(list.begin(), list.end(),
              [](const GeometricObject *a, const GeometricObject *b)
                  -> bool { return a->priority < b->priority; });
  }

  void remove(const GeometricObject *obj)
  {
    auto it = find(list.begin(), list.end(), obj);
    if (it != list.end()) {
      list.erase(it);
    }
  }

  void clear() { list.clear(); }

  size_t size() const { return list.size(); }
};

float radiance(const Scene &scene, Ray &ray, float wavelen, Sampler &sampler,
               int sample_index, int nested, InteriorList &interior)
{
  if (nested > 100) return 0.0f;
  bool hit = false;
  for (const auto &o : scene.objects) {
    Ray oray = {inverse_transform_point(o.xform, ray.origin),
                inverse_transform_vector(o.xform, ray.direction), ray.tmax,
                ray.originator};
    if (o.shape->intersect(oray, oray.originator == &o, interior.has(&o))) {
      ray.hit_object = &o;
      hit = true;
      ray.tmax = oray.tmax;
      ray.position = transform_point(o.xform, oray.position);
      ray.normal = normalize(transform_normal(o.xform, oray.normal));
    }
  }
  if (!hit) {
    return scene.skybox->sample(ray.direction, wavelen);
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
      outer_refractive_index =
          interior.next_top()->mat->refractive_index.sample(wavelen);
    }

    mat3 from_tangent = basis_from_normal(ray.normal);
    mat3 to_tangent = inverse(from_tangent);

    vec3 wo_t = mul(to_tangent, -ray.direction);
    // vec3 wi_t = vec3(0, 0, 1);
    // vec3 wi_t = vec3(-wo_t.x, -wo_t.y, wo_t.z);
    // vec3 sample = sampler.shading[sample_index];
    vec3 wi_t;
    float pdf;
    // float fr = brdf_lambertian(wo_t, wi_t, pdf, sample[0], sample[1]);
    // float fr = brdf_lambertian(wo_t, wi_t, pdf, frand(), frand());
    float fr = ray.hit_object->mat->fr(
        wo_t, wi_t, wavelen, outer_refractive_index, pdf, frand(), frand());
    wi_w = mul(from_tangent, wi_t);

    // SolidXyzCurveSpectrum R = { srgb_to_xyz(vec3{0.0, 1.0, 0.0}) };
    // Spectrum R = { 400, 700, {0,1,1,0} };
    Spectrum R(vec3(1, 1, 1));

    factor = R.sample(wavelen) * fr * abs_cos_theta(wi_t) / pdf;
  }
  else {
    wi_w = ray.direction;
    factor = 1.0;
  }

  if (dot(wi_w, ray.normal) > 0) {
    interior.remove(ray.hit_object);
  }

  Ray next_ray = {ray.position, wi_w, 1000.0, ray.hit_object};
  float L = radiance(scene, next_ray, wavelen, sampler, sample_index,
                     nested + 1, interior);

  return factor * absorbtion * L;
}

float radiance(const Scene &scene, Ray &ray, float wavelen, Sampler &sampler,
               int sample_index)
{
  InteriorList interior;
  return radiance(scene, ray, wavelen, sampler, sample_index, 0, interior);
}
