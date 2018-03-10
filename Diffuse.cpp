#include "Diffuse.h"

const double PI = 3.14;

thread_local std::mt19937 Diffuse::generator = std::mt19937(time(NULL));
thread_local std::uniform_real_distribution<double> Diffuse::distribution = std::uniform_real_distribution<double>(0.0, 1.0);

Diffuse::Diffuse(Color4dRGB c): color(c) {}

double Diffuse::brdf(const Vector3d&, const Vector3d&, const Vector3d&) {
    return 1/PI;
}

double Diffuse::pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return n.dot(wi)/PI;
}

Vector3d Diffuse::importance_sampling(const Vector3d&, const Vector3d& n) {
    double r1 = distribution(generator), r2 = distribution(generator);
    double R = sqrt(r1);
    double theta = 2*PI*r2;
    Vector3d u = n.unitOrthogonal(), v = u.cross(n);

    Vector3d new_dir = Vector3d(cos(theta)*R, sin(theta)*R, sqrt(1-r1));
    Matrix3d rot_objspc_to_wrldspc;
    rot_objspc_to_wrldspc<<u,v,n;
    new_dir = rot_objspc_to_wrldspc*new_dir;

    return new_dir;
}

// brdf = (color / PI) in diffuse case
Color4dRGB Diffuse::explicit_eval(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return color*n.dot(wi)/PI;
}

// Cosine weighted sampling, pdf becomes cos(theta)/PI
// indirect * cos(theta) * brdf / pdf = indirect * color * cos(theta) / PI / (cos(theta) / PI)

void Diffuse::eval(Ray& r, const Vector3d& p, const Vector3d& n) {
    double r1 = distribution(generator), r2 = distribution(generator);
    double R = sqrt(r1);
    double theta = 2*PI*r2;
    Vector3d u = n.unitOrthogonal(), v = u.cross(n);

    Vector3d new_dir = Vector3d(cos(theta)*R, sin(theta)*R, sqrt(1-r1));
    Matrix3d rot_objspc_to_wrldspc;
    rot_objspc_to_wrldspc<<u,v,n;
    new_dir = rot_objspc_to_wrldspc*new_dir;

    r.origin = p+n*0.01;
    r.color = r.color*color;
    r.dir = new_dir;
}


Diffuse::~Diffuse() {}
