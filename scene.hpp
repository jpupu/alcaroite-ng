#pragma once
#include "pupumath_struct.hpp"
#include <memory>
#include <vector>

class Shape;
class Material;
class Skybox;

class GeometricObject {
public:
  std::shared_ptr<Shape> shape;

  std::shared_ptr<Material> mat;

  int priority;

  /// Object to world transformation.
  pupumath::Transform xform;
};

struct Scene {
  std::vector<GeometricObject> objects;
  std::shared_ptr<Skybox> skybox;
};
