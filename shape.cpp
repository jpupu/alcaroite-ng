#include "shape.hpp"
#include "pupumath.hpp"
#include "ray.hpp"
#include "ValueBlock.hpp"
#include <vector>
using namespace pupumath;

class Sphere : public Shape {
public:
  bool intersect(Ray& ray, bool is_originator, bool inside_originator) const
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

  bool intersect(Ray& ray, bool is_originator, bool inside_originator) const
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
  bool intersect(Ray& ray, bool is_originator, bool inside_originator) const
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

class DirectQuad : public Shape {
public:
  DirectQuad(vec3 v[4]) : v{v[0], v[1], v[2], v[3]} {}

  vec3 v[4];

  bool intersect(Ray& ray, bool is_originator, bool inside_originator) const
  {
    // An Efficient Ray-Quadrilateral Intersection Test
    // Area Lagae, Philip Dutré

    // Reject rays using the barycentric coordinates of
    // the intersection point with respect to T.
    vec3 e01 = v[1] - v[0];
    vec3 e03 = v[3] - v[0];
    vec3 p = cross(ray.direction, e03);
    float det = dot(e01, p);
    if (fabs(det) == 0) return false;

    vec3 T = ray.origin - v[0];
    float a = dot(T, p) / det;
    if (a < 0 || a > 1) return false;

    vec3 q = cross(T, e01);
    float b = dot(ray.direction, q) / det;
    if (b < 0 || b > 1) return false;

    // Reject rays using the barycentric coordinates of
    // the intersection point with respect to T'.
    if ((a + b) > 1) {
      vec3 e23 = v[3] - v[2];
      vec3 e21 = v[1] - v[2];
      vec3 p = cross(ray.direction, e21);
      float det = dot(e23, p);
      if (fabs(det) == 0) return false;

      vec3 T = ray.origin - v[2];
      float a = dot(T, p) / det;
      if (a < 0 || a > 1) return false;

      vec3 q = cross(T, e23);
      float b = dot(ray.direction, q) / det;
      if (b < 0 || b > 1) return false;
    }

    // Compute the ray parameter of the intersection point.
    float t = dot(e03, q) / det;
    if (t < 0.0f) return false;
    if (t > ray.tmax) return false;

    if (is_originator) {
      if (inside_originator && ray.direction.y < 0) return false;
      if (!inside_originator && ray.direction.y > 0) return false;
    }

    ray.tmax = t;
    ray.position = ray.origin + ray.direction * t;
    ray.normal = vec3(0, 1, 0);

    return true;
  }
};

class QuadMesh : public Shape {
public:
  QuadMesh(std::vector<vec3> v, std::vector<int> f) : vertdata(v), facedata(f)
  {
  }

  std::vector<vec3> vertdata;
  std::vector<int> facedata;

  bool intersect(Ray& ray, bool is_originator, bool inside_originator) const
  {
    float t = 0;
    bool hit = false;

    for (size_t i = 0; i < facedata.size() / 4; ++i) {
      vec3 v0 = vertdata[facedata[i * 4 + 0]];
      vec3 v1 = vertdata[facedata[i * 4 + 1]];
      vec3 v2 = vertdata[facedata[i * 4 + 2]];
      vec3 v3 = vertdata[facedata[i * 4 + 3]];

      // Reject rays using the barycentric coordinates of
      // the intersection point with respect to T.
      vec3 e01 = v1 - v0;
      vec3 e03 = v3 - v0;
      vec3 p = cross(ray.direction, e03);
      float det = dot(e01, p);
      if (fabs(det) == 0) continue;

      vec3 T = ray.origin - v0;
      float a = dot(T, p) / det;
      if (a < 0 || a > 1) continue;

      vec3 q = cross(T, e01);
      float b = dot(ray.direction, q) / det;
      if (b < 0 || b > 1) continue;

      // Reject rays using the barycentric coordinates of
      // the intersection point with respect to T'.
      if ((a + b) > 1) {
        vec3 e23 = v3 - v2;
        vec3 e21 = v1 - v2;
        vec3 p = cross(ray.direction, e21);
        float det = dot(e23, p);
        if (fabs(det) == 0) continue;

        vec3 T = ray.origin - v2;
        float a = dot(T, p) / det;
        if (a < 0 || a > 1) continue;

        vec3 q = cross(T, e23);
        float b = dot(ray.direction, q) / det;
        if (b < 0 || b > 1) continue;
      }

      // Compute the ray parameter of the intersection point.
      t = dot(e03, q) / det;
      if (t < 0.0f) continue;
      if (t > ray.tmax) continue;

      if (is_originator) {
        if (inside_originator && ray.direction.y < 0) continue;
        if (!inside_originator && ray.direction.y > 0) continue;
      }

      hit = true;
    }

    if (!hit) return false;

    ray.tmax = t;
    ray.position = ray.origin + ray.direction * t;
    ray.normal = vec3(0, 1, 0);

    return true;
  }
};

std::shared_ptr<Shape> build_shape(const ValueBlock& block)
{
  auto type = block.get<std::string>("type");
  if (type == "sphere") {
    return std::make_shared<Sphere>();
  }
  else if (type == "plane") {
    return std::make_shared<Plane>();
  }
  else if (type == "direct_quad") {
    vec3 v[4] = {block.get<vec3>("v0"), block.get<vec3>("v1"),
                 block.get<vec3>("v2"), block.get<vec3>("v3")};
    return std::make_shared<DirectQuad>(v);
  }
  else if (type == "quadmesh") {
    // std::vector<vec3> v = {block.get<vec3>("v0"), block.get<vec3>("v1"),
    //              block.get<vec3>("v2"), block.get<vec3>("v3"),
    //              block.get<vec3>("v4"), block.get<vec3>("v5"),
    //              block.get<vec3>("v6"), block.get<vec3>("v7")};
    return std::make_shared<QuadMesh>(block.get<std::vector<vec3>>("vertices"),
                                      block.get<std::vector<int>>("faces"));
  }
  else {
    throw std::runtime_error("unknown shape");
  }
}
