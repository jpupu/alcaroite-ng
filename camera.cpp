#include "camera.hpp"
#include "pupumath.hpp"
#include "ValueBlock.hpp"
using namespace pupumath;

namespace camera_ns {
class Pinhole : public Camera {
public:
  float film_dist;
  vec3 origin;

  Pinhole(Transform cam_to_world, float fov = 90)
      : Camera(cam_to_world), origin(transform_point(cam_to_world, vec3(0)))
  {
    set_fov(fov);
  }

  void set_fov(float vertical_fov)
  {
    // Divide by two since the given FOV includes both symmetrical sides.
    float a = (vertical_fov / 2) / 180 * M_PI;
    film_dist = 1.0 / tan(a);
  }

  CameraSample project(const vec3& film_sample, float wavelen,
                       const vec3& lens_sample) const override
  {
    auto dir = vec3{film_sample.x, film_sample.y, -film_dist};
    return {origin, transform_normal(cam_to_world, normalize(dir))};
  }
};
} // camera_ns

using namespace camera_ns;

Camera::Camera(Transform cam_to_world) : cam_to_world(cam_to_world) {}

std::shared_ptr<Camera> build_camera(const ValueBlock& block)
{
  Transform xform = {block.get<mat34>("transform"),
                     block.get<mat34>("inverse_transform")};

  auto type = block.get<std::string>("type");
  if (type == "pinhole") {
    return std::make_shared<Pinhole>(xform, block.get<double>("fov"));
  }
  else {
    throw std::runtime_error("unknown camera");
  }
}
