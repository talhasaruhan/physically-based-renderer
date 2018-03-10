#pragma once
#include "Material.h"

// Perfect refractive material
class Refractive:
    public Material {
public:
    Refractive(double n = 1.0);

    double ior;

    double brdf(const Vector3d&, const Vector3d&, const Vector3d&);
    Vector3d importance_sampling(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&, const Vector3d&);

    Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&);

    void eval(Ray&, const Vector3d&, const Vector3d&);

    ~Refractive();
};
