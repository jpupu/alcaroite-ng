#pragma once
#include "pupumath.hpp"

class GeometricObject;

struct Ray {
  const pupumath::vec3 origin;
  const pupumath::vec3 direction;

  // constraints:
  float tmax;
  const GeometricObject *originator; // the object that the ray originates from
  // int originator_subid;

  // intersection:
  pupumath::vec3 position;
  pupumath::vec3 normal;
  // vec3 texcoord;
  // Material* material;
  const GeometricObject *hit_object;
  // int hit_subid;

  // context: [wrong place?]
  // stack<Object*> media;
};
