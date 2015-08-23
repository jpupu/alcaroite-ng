#pragma once
#include "pupumath_struct.hpp"
#include <memory>

class ValueBlock;

struct CameraSample {
  const pupumath::vec3 origin;
  const pupumath::vec3 direction;
};

class Camera {
public:
  pupumath::Transform cam_to_world;

  Camera(pupumath::Transform cam_to_world);

  virtual CameraSample project(const pupumath::vec3& film_sample, float wavelen,
                               const pupumath::vec3& lens_sample) const = 0;
};

std::shared_ptr<Camera> build_camera(const ValueBlock& block);
