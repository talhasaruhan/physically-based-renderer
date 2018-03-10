#include "Reflective.h"

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

Reflective::Reflective() {}

double Reflective::brdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return ((2*n.dot(wo)*n-wo).dot(wi)>0.999) ? 1.0 : 0.0;
}

double Reflective::pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return ((2*n.dot(wo)*n-wo).dot(wi)>0.999) ? 1.0 : inf;
}

Color4dRGB Reflective::explicit_eval(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return Color4dRGB(1.0);
}

Vector3d Reflective::importance_sampling(const Vector3d& wo, const Vector3d& n) {
    return Vector3d::Identity();
}

void Reflective::eval(Ray& r, const Vector3d& p, const Vector3d& n) {
    r.origin = p+n*0.01;
    r.color = r.color;

    Vector3d o = (-2*r.dir.dot(n)*n+r.dir);
    r.dir = o;
}


Reflective::~Reflective() {}
