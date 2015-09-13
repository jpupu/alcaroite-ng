#include "ray.hpp"
#include <cstdio>

struct debug_t {
  bool enabled;
  int nest_level;

  void log (const Ray&);
  void miss ();
  void hit (const Ray&);
  void shading (const pupumath::vec3& wo, const pupumath::vec3& wi, bool tangent);

  template<typename... Args>
  void log (const char* fmt, Args... args)
  {
    if (!enabled) return;
    printf("%*s", nest_level, "");
    printf(fmt, args...);
    printf("\n");
  }
};

extern debug_t debug;

struct DebugNester
{
  DebugNester() { debug.nest_level++; }
  ~DebugNester() { debug.nest_level--; }
};

