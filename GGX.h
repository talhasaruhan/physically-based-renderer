#pragma once
#include "Material.h"
#include <random>

// GGX Reflective Microfacet Model (No transmission)
class GGX:
    public Material {
public:
    double roughness;
    double ior;
    Color4dRGB color;
    static thread_local std::mt19937 generator;
    static thread_local std::uniform_real_distribution<double> distribution;

    GGX(double alpha = 0.2, Color4dRGB c = Color4dRGB(0.8, 0.6, 0.4), double n = 1.0);

    double D(const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&);
    inline double G1(const Vector3d&, const Vector3d&, const Vector3d&);
    double G(const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&);
    double F(const Vector3d&, const Vector3d&, const Vector3d&);

    inline Vector3d sample_microfacet_normal(const Vector3d&);

    double brdf(const Vector3d&, const Vector3d&, const Vector3d&);
    Vector3d importance_sampling(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&, const Vector3d&);

    Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&);

    void eval(Ray&, const Vector3d&, const Vector3d&);

    ~GGX();
};
