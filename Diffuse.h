#pragma once
#include "Material.h"
#include <random>

// Lambertian Diffuse
class Diffuse:
    public Material {
public:
    Color4dRGB color;
    static thread_local std::mt19937 generator;
    static thread_local std::uniform_real_distribution<double> distribution;

    Diffuse(Color4dRGB c = Color4dRGB(0.8, 0.6, 0.4));

    double brdf(const Vector3d&, const Vector3d&, const Vector3d&);
    Vector3d importance_sampling(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&, const Vector3d&);

    Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&);

    void eval(Ray&, const Vector3d&, const Vector3d&);

    ~Diffuse();
};
