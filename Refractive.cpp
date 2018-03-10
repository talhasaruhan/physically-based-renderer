#include "Refractive.h"

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

Refractive::Refractive(double n): ior(n) {}

double Refractive::brdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return 1.0;
}

double Refractive::pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return 1.0;
}

Color4dRGB Refractive::explicit_eval(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return 1.0;
}

Vector3d Refractive::importance_sampling(const Vector3d& wo, const Vector3d& n) {
    return Vector3d(0.0, 0.0, 0.0);
}

// normal faces away from the volume
void Refractive::eval(Ray& r, const Vector3d& p, const Vector3d& n) {
    double chi = -1.0;
    if(n.dot(r.dir)>0){
        chi = 1.0;
    }

    bool inside = chi>0;

    double ni, no;
    if(inside) {
        ni = ior;
        no = 1.0;
    } else {
        ni = 1.0;
        no = ior;
    }
    double n_ratio = ni/no;

    double cos_theta_i = r.dir.dot(n);
    double sin_theta_i = sqrt(1-cos_theta_i*cos_theta_i);
    bool total_internal_refl = (n_ratio > 1.0) ? n_ratio*sin_theta_i>1.0 : false;
    
    Vector3d o;
    if(!total_internal_refl) {
        o = chi*sqrt(1-n_ratio*n_ratio*(1-cos_theta_i*cos_theta_i))*n+
            n_ratio*(r.dir-cos_theta_i*n);
    } else {
        o = (-2*r.dir.dot(n)*n+r.dir);
    }

    r.origin = p+o*0.01;
    r.dir = o;

}

Refractive::~Refractive() {}
