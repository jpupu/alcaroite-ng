#include "debug.hpp"
#include <cstdio>

debug_t debug;

void debug_t::log(const Ray& ray)
{
  if (!enabled) return;
  printf("%*s☉(%.3f, %.3f, %.3f) →(%.3f, %.3f, %.3f)\n", nest_level, "",
      ray.origin.x, ray.origin.y, ray.origin.z,
      ray.direction.x, ray.direction.y, ray.direction.z);
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
      ray.hit_object,
      ray.position.x, ray.position.y, ray.position.z,
      ray.normal.x, ray.normal.y, ray.normal.z);
}

void debug_t::shading (const pupumath::vec3& wo, const pupumath::vec3& wi, bool tangent){
  if (!enabled) return;
  printf("%*s⌘ shading %s wo(%.3f, %.3f, %.3f) wi(%.3f, %.3f, %.3f)\n",
      nest_level, "",
      tangent ? "⊥" : "✝",
      wo.x, wo.y, wo.z,
      wi.x, wi.y, wi.z);
}
