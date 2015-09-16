#include "ray.hpp"
#include <cstdio>
#include <vector>

struct debug_t {
  bool enabled;
  int nest_level;
  int paths;
  int rays;
  int total_rays;
  int max_path_length;
  int min_path_length;
  std::vector<int> path_lengths;

  debug_t();
  void begin_path();
  void end_path();
  void ray(const Ray&);
  void miss();
  void hit(const Ray&);
  void shading(const pupumath::vec3& wo, const pupumath::vec3& wi,
               bool tangent);

  template <typename... Args>
  void log(const char* fmt, Args... args)
  {
    if (!enabled) return;
    printf("%*s", nest_level, "");
    printf(fmt, args...);
    printf("\n");
  }
};

extern debug_t debug;

struct DebugNester {
  DebugNester() { debug.nest_level++; }
  ~DebugNester() { debug.nest_level--; }
};
