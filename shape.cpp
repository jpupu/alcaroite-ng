#include "shape.hpp"
#include "pupumath.hpp"
#include "ray.hpp"
#include "ValueBlock.hpp"
using namespace pupumath;

class Sphere : public Shape {
public:
  bool intersect(Ray &ray, bool is_originator, bool inside_originator) const
  {
    float a = dot(ray.direction, ray.direction);
    float b = 2 * dot(ray.origin, ray.direction);
    float c = dot(ray.origin, ray.origin) - 1.0;
    float d = b * b - 4 * a * c;

    if (d < 0) {
      return false;
    }

    // float t1 = (-b - sqrtf(d)) / (2.0 * a);
    // float t2 = (-b + sqrtf(d)) / (2.0 * a);
    float t1 = (b + sqrtf(d)) / (-2.0 * a);
    float t2 = (b - sqrtf(d)) / (-2.0 * a);

    float t;
    if (is_originator) {
      t = inside_originator ? t2 : t1;
    }
    else {
      t = (t1 < 0.0f) ? t2 : t1;
    }

    // float t = t1;
    // if (t > ray.tmax) return false;
    // if (t < 0.0f) t = t2;
    if (t < 0.0f) return false;
    if (t > ray.tmax) return false;

    ray.tmax = t;
    ray.position = ray.origin + ray.direction * t;
    ray.normal = normalize(ray.position);

    return true;
  }
};

class ScaledSphere : public Shape {
public:
  ScaledSphere(float r) : radius(r) {}

  float radius;

  bool intersect(Ray &ray, bool is_originator, bool inside_originator) const
  {
    float a = dot(ray.direction, ray.direction);
    float b = 2 * dot(ray.origin, ray.direction);
    float c = dot(ray.origin, ray.origin) - radius * radius;
    float d = b * b - 4 * a * c;

    if (d < 0) {
      return false;
    }

    // float t1 = (-b - sqrtf(d)) / (2.0 * a);
    // float t2 = (-b + sqrtf(d)) / (2.0 * a);
    float t1 = (b + sqrtf(d)) / (-2.0 * a);
    float t2 = (b - sqrtf(d)) / (-2.0 * a);

    float t;
    if (is_originator) {
      t = inside_originator ? t2 : t1;
    }
    else {
      t = (t1 < 0.0f) ? t2 : t1;
    }

    // float t = t1;
    // if (t > ray.tmax) return false;
    // if (t < 0.0f) t = t2;
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
  bool intersect(Ray &ray, bool is_originator, bool inside_originator) const
  {
    if (ray.direction.y == 0) return false;

    if (is_originator) {
      if (inside_originator && ray.direction.y < 0) return false;
      if (!inside_originator && ray.direction.y > 0) return false;
    }

    float t = -ray.origin.y / ray.direction.y;

    if (t < 0.0f) return false;
    if (t > ray.tmax) return false;

    ray.tmax = t;
    ray.position = ray.origin + ray.direction * t;
    ray.normal = vec3(0, 1, 0);

    return true;
  }
};

std::shared_ptr<Shape> build_shape(const ValueBlock &block)
{
  auto type = block.get<std::string>("type");
  if (type == "sphere") {
    return std::make_shared<Sphere>();
  }
  else if (type == "plane") {
    return std::make_shared<Plane>();
  }
  else {
    throw std::runtime_error("unknown shape");
  }
}
