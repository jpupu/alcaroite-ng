#include "debug.hpp"
#include <algorithm>
#include <cstdio>


debug_t debug;

debug_t::debug_t()
    : enabled(false), nest_level(0), paths(0), rays(0), total_rays(0),
      max_path_length(0), min_path_length(1000000)
{
}

void debug_t::begin_path()
{
  ++paths;
  rays = 0;
}

void debug_t::end_path()
{
  // max_path_length = std::max(max_path_length, rays);
  min_path_length = std::min(min_path_length, rays);
  if (rays > max_path_length) {
    path_lengths.resize(rays+1, 0);
    max_path_length = rays;
  }
  ++path_lengths[rays];
}

void debug_t::ray(const Ray& ray)
{
  ++rays;
  ++total_rays;
  if (!enabled) return;
  printf("%*s☉(%.3f, %.3f, %.3f) →(%.3f, %.3f, %.3f)\n", nest_level, "",
         ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x,
         ray.direction.y, ray.direction.z);
}

void debug_t::miss()
{
  if (!enabled) return;
  printf("%*s☄ miss\n", nest_level, "");
}

void debug_t::hit(const Ray& ray)
{
  if (!enabled) return;
  printf("%*s✗ %p ☉(%.3f, %.3f, %.3f) ⊥(%.3f, %.3f, %.3f)\n", nest_level, "",
         ray.hit_object, ray.position.x, ray.position.y, ray.position.z,
         ray.normal.x, ray.normal.y, ray.normal.z);
}

void debug_t::shading(const pupumath::vec3& wo, const pupumath::vec3& wi,
                      bool tangent)
{
  if (!enabled) return;
  printf("%*s⌘ shading %s wo(%.3f, %.3f, %.3f) wi(%.3f, %.3f, %.3f)\n",
         nest_level, "", tangent ? "⊥" : "✝", wo.x, wo.y, wo.z, wi.x, wi.y,
         wi.z);
}
