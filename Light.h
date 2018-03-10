#pragma once
#include <Eigen/Dense>
#include "Material.h"
#include "Emissive.h"
#include "Entity.h"
#include "Color.h"
#include "Ray.h"
#include "Primitives.h"

using namespace Eigen;

class Light {
public:
    int id;
    Color4dRGB color;

    Light(const Color4dRGB&);

    virtual Vector3d sample_area(const Vector3d&) = 0;

    virtual double weight(const Vector3d&, const Vector3d&) = 0;
    virtual double pdf(const Vector3d&, const Vector3d&) = 0;
    // summand is : color * brdf / pdf; brdf = 1 for lights, (1/pdf) = weight
    virtual Color4dRGB direct_light_sample(const Vector3d&, const Vector3d&) = 0;

    virtual Vector3d normal(const Vector3d&) = 0;
    virtual Vector3d intersection_point(const Ray&) = 0;
    virtual double intersection_distance(const Ray&) = 0;

    ~Light();
};

class PointLight:
    public Light {
public:
    Vector3d position;
    bool soft_shadow;
    double radius, power;
    static thread_local std::mt19937 generator;
    static thread_local std::uniform_real_distribution<double> distribution;

    PointLight(const Vector3d&, const Color4dRGB& c = Color4dRGB(1.0), double power = 1.0, double radius = 0.0,
               bool ss = false);

    Vector3d intersection_point(const Ray&);
    double intersection_distance(const Ray&);
    Vector3d sample_area(const Vector3d&);
    Vector3d normal(const Vector3d&);
    double weight(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&);
    Color4dRGB direct_light_sample(const Vector3d&, const Vector3d&);

    ~PointLight();
};

class AreaLight:
    public Light {
public:
    Vector3d p0;
    Vector3d p1;
    Vector3d p2;
    Vector3d p3;
    Vector3d normal_;
    double area;
    static thread_local std::mt19937 generator;
    static thread_local std::uniform_real_distribution<double> distribution;

    AreaLight(const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&, const Color4dRGB&);

    Vector3d intersection_point(const Ray&);
    double intersection_distance(const Ray&);
    Vector3d sample_area(const Vector3d&);
    Vector3d normal(const Vector3d&);
    double weight(const Vector3d&, const Vector3d&);
    double pdf(const Vector3d&, const Vector3d&);
    Color4dRGB direct_light_sample(const Vector3d&, const Vector3d&);

    ~AreaLight();
};
