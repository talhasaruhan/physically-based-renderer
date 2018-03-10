#include "Emissive.h"

const double PI = 3.14;

Emissive::Emissive(Color4dRGB c): color(c) {}

double Emissive::brdf(const Vector3d&, const Vector3d&, const Vector3d&) {
    return 1.0;
}

double Emissive::pdf(const Vector3d& wi, const Vector3d& wo, const Vector3d& n) {
    return 1.0;
}

Vector3d Emissive::importance_sampling(const Vector3d&, const Vector3d& n) {
    return Vector3d(n);
}

Color4dRGB Emissive::explicit_eval(const Vector3d&, const Vector3d&, const Vector3d&) {
    return color;
}

// monte-carlo integrator takes the view angle into account, don't double dip
void Emissive::eval(Ray& r, const Vector3d& p, const Vector3d& n) {
    r.color = r.color*color;
    r.terminated = true;
}

Emissive::~Emissive() {}
