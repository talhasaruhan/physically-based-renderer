#include "Light.h"

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

thread_local std::mt19937 AreaLight::generator = std::mt19937(time(NULL));
thread_local std::uniform_real_distribution<double> AreaLight::distribution = std::uniform_real_distribution<double>(0.0, 1.0);
thread_local std::mt19937 PointLight::generator = std::mt19937(time(NULL));
thread_local std::uniform_real_distribution<double> PointLight::distribution = std::uniform_real_distribution<double>(0.0, 1.0);

Light::Light(const Color4dRGB& c):
    color(c) {}

Light::~Light() {}

/* area light source */

AreaLight::AreaLight(const Vector3d& a, const Vector3d& b, const Vector3d& c, const Vector3d& d, const Color4dRGB& clr):
    Light(clr), p0(a), p1(b), p2(c), p3(d), normal_((p1-p0).cross(p3-p0).normalized()),
    area(((p1-p0).cross(p3-p0)).norm()) {}

AreaLight::~AreaLight() {}

Vector3d AreaLight::intersection_point(const Ray& r) {
    Tri a = Tri(nullptr, p0, p1, p2);
    Tri b = Tri(nullptr, p0, p2, p3);
    double a_ = a.intersection_distance(r);
    double b_ = b.intersection_distance(r);
    if(a_<inf&&a_>0) return Vector3d(r.origin+r.dir*a_);
    if(b_<inf&&b_>0) return Vector3d(r.origin+r.dir*b_);
    return Vector3d(inf, inf, inf);
}

double AreaLight::intersection_distance(const Ray& r) {
    Tri a = Tri(nullptr, p0, p1, p2);
    Tri b = Tri(nullptr, p0, p2, p3);
    double a_ = a.intersection_distance(r);
    double b_ = b.intersection_distance(r);
    if(a_<inf&&a_>0) return a_;
    if(b_<inf&&b_>0) return b_;
    return inf;
}

Vector3d AreaLight::sample_area(const Vector3d&) {
    Vector3d u = p1-p0, v = p3-p0;
    double r1 = distribution(generator), r2 = distribution(generator);
    return p0+u*r1+v*r2;
}

Vector3d AreaLight::normal(const Vector3d&) {
    return normal_;
}

// fix this for the objects too close to the light source. 
// reduced intensity due to grazing angles with most of the light source samples
// if the source is too close, area doesn't matter
double AreaLight::weight(const Vector3d& p_origin, const Vector3d& p_light) {
    return area/((p_light-p_origin).squaredNorm())*abs(normal(p_light).dot((p_light-p_origin).normalized()));
}

double AreaLight::pdf(const Vector3d& p_origin, const Vector3d& p_light) {
    return 1/weight(p_origin, p_light);
}

Color4dRGB AreaLight::direct_light_sample(const Vector3d& p_origin, const Vector3d& p_light) {
    return color*weight(p_origin, p_light);
}

/* spherical light source */

PointLight::PointLight(const Vector3d& pos, const Color4dRGB& c, double p, double r, bool ss):
    Light(c), position(pos), power(p), radius(r), soft_shadow(ss) {}

Vector3d PointLight::intersection_point(const Ray& ih) {
    Vector3d D(position(0)-ih.origin(0), position(1)-ih.origin(1), position(2)-ih.origin(2));
    double rt = D.dot(ih.dir);
    if(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))>=0) {
        double t1 = (rt+sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        double t2 = (rt-sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        if(fmax(t1, t2)<0) return Vector3d(inf, inf, inf);
        double t;
        if(fmin(t1, t2)>=0) t = fmin(t1, t2);
        else t = fmax(t1, t2);
        return Vector3d(ih.origin+ih.dir*t);
    } else {
        return Vector3d(inf, inf, inf);
    }
}

double PointLight::intersection_distance(const Ray& ih) {
    Vector3d D(position(0)-ih.origin(0), position(1)-ih.origin(1), position(2)-ih.origin(2));
    double rt = D.dot(ih.dir);
    if(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))>=0) {
        double t1 = (rt+sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        double t2 = (rt-sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        if(fmax(t1, t2)<0) return inf;
        double t;
        if(fmin(t1, t2)>0) t = fmin(t1, t2);
        else t = fmax(t1, t2);
        return t*ih.dir.norm();
    } else {
        return inf;
    }
}

Vector3d PointLight::sample_area(const Vector3d& p) {
    if(soft_shadow) {
        double u1 = 2*distribution(generator)-1, u2 = distribution(generator);
        double v = sqrt(1.0f-u1*u1);
        double phi = 2*PI*u2;

        Vector3d w = Vector3d(cos(phi)*v, sin(phi)*v, u1);

        if(w.dot(position-p)>0)
            w *= -1;

        // bug: sample only visible portion of the sphere's surface

        return position+w*radius;
    }

    return position;
}

Vector3d PointLight::normal(const Vector3d& p_light) {
    return (p_light-position).normalized();
}

// R^2 / r^2 * N.L
double PointLight::weight(const Vector3d& p_origin, const Vector3d& p_light) {
    return radius*radius/((p_light-p_origin).squaredNorm())*normal(p_light).dot((p_origin-p_light).normalized());
}

double PointLight::pdf(const Vector3d& p_origin, const Vector3d& p_light) {
    return 1/weight(p_origin, p_light);
}

Color4dRGB PointLight::direct_light_sample(const Vector3d& p_origin, const Vector3d& p_light) {
    return color*power*weight(p_origin, p_light);
}

PointLight::~PointLight() {}
