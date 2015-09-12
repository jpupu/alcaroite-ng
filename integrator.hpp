#pragma once

class Ray;
class Sample;
struct Scene;

float radiance(const Scene& scene, Ray& ray, float wavelen, Sample& sample);
