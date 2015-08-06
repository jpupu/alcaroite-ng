#pragma once

class Ray;
class Sampler;
struct Scene;

float radiance(const Scene &scene, Ray &ray, float wavelen, Sampler &sampler,
               int sample_index);
