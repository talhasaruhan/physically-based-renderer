#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <time.h>

class Material {
public:
    Material();

    // note that in order to allow cancelling terms, cosine term in rendering equation is calculated by the material itself

    // convention:

    // wo: viewing vector
    // wi: light vector
    // n: surface normal

    // on many functions, this notation is reversed, as we name sampled ray outgoing, which becomes a light ray, thus noted as wi
    // i is used for incoming ray direction, which for our purposes, is wo (camera ray direction)
    // and o is used for sampled direction, which is wi, (possible)light direction

    // sample lights?
    //bool direct_sample();

    // sample according to the importance distribution
    virtual Vector3d importance_sampling(const Vector3d&, const Vector3d&) = 0;

    // brdf
    virtual double brdf(const Vector3d&, const Vector3d&, const Vector3d&) = 0;

    // pdf
    virtual double pdf(const Vector3d&, const Vector3d&, const Vector3d&) = 0;

    // color * brdf * n.dot(wi)
    virtual Color4dRGB explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&) = 0;

    // importance sample, summand: color * brdf * n.dot(wi) / pdf
    virtual void eval(Ray&, const Vector3d&, const Vector3d&) = 0;
    ~Material();
};

#endif
