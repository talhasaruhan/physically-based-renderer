#pragma once
#include "Material.h"
#include <random>

// perfect mirror
class Reflective:
    public Material {
public:
    Reflective();

    double brdf(const Vector3d&, const Vector3d&, const Vector3d&);
    Vector3d importance_sampling(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&, const Vector3d&);

    Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&);

    void eval(Ray&, const Vector3d&, const Vector3d&);

    ~Reflective();
};
