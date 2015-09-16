#include "camera.hpp"
#include "debug.hpp"
#include "framebuffer.hpp"
#include "integrator.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sampler.hpp"
#include "scene.hpp"
#include "shape.hpp"
#include "skybox.hpp"
#include "spectrum.hpp"
#include "pupumath.hpp"
#include "ValueBlock.hpp"
#include "util.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <tclap/CmdLine.h>
using namespace pupumath;

Scene build_scene(const std::vector<ValueBlock>& blocks)
{
  Scene scene;
  std::map<std::string, std::shared_ptr<Material>> materials;
  std::map<std::string, std::shared_ptr<Shape>> shapes;
  for (const auto& block : blocks) {

    if (block.type == "material") {
      materials[block.id] = build_material(block);
    }
    else if (block.type == "shape") {
      shapes[block.id] = build_shape(block);
    }
    else if (block.type == "skybox") {
      scene.skybox = build_skybox(block);
    }
    else if (block.type == "object") {
      auto shape = shapes.at(block.get<std::string>("shape"));
      auto material = materials.at(block.get<std::string>("material"));
      auto xform_matrix = block.get<mat34>("transform");
      scene.objects.push_back({shape,
                               material,
                               static_cast<int>(block.get<double>("priority")),
                               {xform_matrix, inverse(xform_matrix)}});
    }
    else if (block.type == "camera") {
      scene.camera = build_camera(block);
    }
  }
  return scene;
}

void test_spectrum()
{
  using namespace spectrum_ns;
  int cols = Spectrum::max + 1 - Spectrum::min;
  Framebuffer framebuffer(cols, 100);
  Spectrum reconstructed(vec3(0));

  for (int x = 0; x < cols; x++) {
    int wl = Spectrum::min + x;
    vec3 xyz = spectrum_sample_to_xyz(wl, 1);
    vec3 lrgb = xyz_to_linear_rgb(xyz);
    for (int y = 0; y < 50; y++) {
      framebuffer.add_sample(x, y, lrgb);
    }
    auto sample_reconstructed = linear_rgb_to_spectrum(lrgb);
    for (int i = 0; i < Spectrum::count; i++) {
      reconstructed.samples[i] += sample_reconstructed[i] / cols;
    }
  }

  for (int x = 0; x < cols; x++) {
    int wl = Spectrum::min + x;
    vec3 xyz = spectrum_sample_to_xyz(wl, reconstructed.sample(wl));
    vec3 lrgb = xyz_to_linear_rgb(xyz);
    for (int y = 50; y < 100; y++) {
      framebuffer.add_sample(x, y, lrgb);
    }
  }

  framebuffer.save_ppm("test_spectrum.ppm");
}

int main(int argc, char* argv[])
{

  // vec3 wo = normalize(vec3(-1,0,-1));
  // vec3 wi;
  // float pdf;
  // btdf_dielectric(wo, wi, pdf, 1.0, 1.5);

  // printf("wo %f %f %f (%f)\n", wo.x, wo.y, wo.z, acosf(fabs(wo.z))/M_PI*180);
  // printf("wi %f %f %f (%f)\n", wi.x, wi.y, wi.z, acosf(fabs(wi.z))/M_PI*180);

  // return 0;

  TCLAP::CmdLine cmd("alcaroite ray tracer", ' ', "alpha", false);
  TCLAP::ValueArg<int> width_arg("w", "width", "Width in pixels", false, 400,
                                 "int", cmd);
  TCLAP::ValueArg<int> height_arg("h", "height", "Height in pixels", false, 400,
                                  "int", cmd);
  TCLAP::ValueArg<int> samples_arg("s", "samples", "Sampler per pixel", false,
                                   40, "int", cmd);
  TCLAP::UnlabeledValueArg<std::string> input_file_arg(
      "input", "Input file", "test.ascn", "???", "scene file", cmd);
  TCLAP::ValueArg<std::string> output_file_arg("o", "output", "Output file",
                                               false, "foo.ppm", "file", cmd);
  TCLAP::ValueArg<std::string> sampler_arg("", "sampler", "Sampler", false,
                                           "lhs", "libcrandom|lhs", cmd);
  TCLAP::SwitchArg test_spectrum_arg("", "test-spectrum", "Test spectrum", cmd);
  TCLAP::SwitchArg help_arg("", "help", "Show this help message", cmd);

  cmd.parse(argc, argv);

  if (help_arg.getValue()) {
    TCLAP::StdOutput().usage(cmd);
    return 0;
  }

  if (test_spectrum_arg.getValue()) {
    test_spectrum();
    return 0;
  }

  std::ifstream infile(input_file_arg.getValue());
  auto blocks = read_valueblock_file(infile);
  // for (const auto& block : blocks) {
  //   std::cout << block;
  // }
  Scene scene = build_scene(blocks);

  int W = width_arg.getValue();
  int H = height_arg.getValue();
  int S = samples_arg.getValue();
  printf("Rendering %dx%d with %d samples/pixel\n", W, H, S);
  Framebuffer framebuffer(W, H);
  std::shared_ptr<Sampler> sampler = create_sampler(S, sampler_arg.getValue());
  auto start = std::chrono::system_clock::now();
// #define SINGLE
#ifdef SINGLE
  constexpr int x0 = 50;
  constexpr int y0 = 70;
  for (int y = y0; y < y0 + 1; y++) {
    for (int x = x0; x < x0 + 1; x++) {
#else
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
#endif
      sampler->generate();
      for (int s = 0; s < S; s++) {
        debug.begin_path();
        debug.enabled = (x == 100 && y == 100 && s == 0);
        auto sample = Sample(sampler.get(), s);
        float wavelen = sample.wavelen();
        CameraSample camsamp =
            scene.camera->project(vec2{(float(x + .5) / W * 2 - 1) * W / H,
                                       -(float(y + .5) / H * 2 - 1)},
                                  wavelen, sample.lens());
        Ray ray = {camsamp.origin, camsamp.direction, 1000.0, nullptr};
        float L = radiance(scene, ray, wavelen, sample);
// printf("--> L=%f\n", L);
#ifdef SINGLE
        L = 1000;
#endif
        debug.end_path();
        vec3 rgb = spectrum_ns::xyz_to_linear_rgb(
            spectrum_ns::spectrum_sample_to_xyz(wavelen, L));
        framebuffer.add_sample(x, y, rgb);
      }
      // printf("%c", '0'+int(9*L));
    }
    // printf("\n");
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  printf("Rendered in %.1f seconds\n", elapsed.count());
  framebuffer.save_ppm(output_file_arg.getValue());

  printf("Total paths: %d\n", debug.paths);
  printf("Total rays: %d\n", debug.total_rays);
  printf("Mean rays/path: %.1f\n", float(debug.total_rays) / debug.paths);
  printf("Min rays/path: %d\n", debug.min_path_length);
  printf("Max rays/path: %d\n", debug.max_path_length);
  // for (int i = 0; i <= debug.max_path_length; ++i) {
  //   printf("percentage of paths of length %d: %.1f\n", i,
  //          100 * float(debug.path_lengths[i]) / debug.paths);
  // }
}
