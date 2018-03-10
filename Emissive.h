#pragma once
#include "Material.h"

// Perfect emissive material
class Emissive:
    public Material {
public:
    Color4dRGB color;

    Emissive(Color4dRGB c = Color4dRGB(1.0));

    double brdf(const Vector3d&, const Vector3d&, const Vector3d&);
    Vector3d importance_sampling(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&, const Vector3d&);

    Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&);
    void eval(Ray&, const Vector3d&, const Vector3d&);

    ~Emissive();
};
