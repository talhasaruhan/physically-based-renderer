#include <algorithm>
#include "Primitives.h"

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

Sphere::Sphere(Object* i, double r, const Vector3d& p):
    Primitive(i), radius(r), position(p) {}

Vector3d Sphere::intersection_point(const Ray& ih) {
    Vector3d D(position(0)-ih.origin(0), position(1)-ih.origin(1), position(2)-ih.origin(2));
    double rt = D.dot(ih.dir);
    if(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))>=0) {
        double t1 = (rt+sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        double t2 = (rt-sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        if(fmax(t1, t2)<0) return Vector3d(inf, inf, inf);
        double t;
        if(fmin(t1, t2)>=0) t = fmin(t1, t2);
        else t = fmax(t1, t2);
        c_normal = Vector3d(ih.origin+ih.dir*t);
        return c_normal;
    } else {
        c_normal = Vector3d(-inf, -inf, -inf);
        return Vector3d(inf, inf, inf);
    }
}

double Sphere::intersection_distance(const Ray& ih) {
    Vector3d D(position(0)-ih.origin(0), position(1)-ih.origin(1), position(2)-ih.origin(2));
    double rt = D.dot(ih.dir);
    if(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))>=0) {
        double t1 = (rt+sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        double t2 = (rt-sqrt(pow(rt, 2)-pow(ih.dir.norm(), 2)*(pow(D.norm(), 2)-pow(radius, 2))))/ih.dir.norm();
        if(fmax(t1, t2)<0) return inf;
        double t;
        if(fmin(t1, t2)>0) t = fmin(t1, t2);
        else t = fmax(t1, t2);
        c_normal = Vector3d(ih.origin+ih.dir*t);
        return t*ih.dir.norm();
    } else {
        c_normal = Vector3d(-inf, -inf, -inf);
        return inf;
    }
}

Vector3d Sphere::normal() const {
    return c_normal;
}

Vector3d Sphere::normal(const Vector3d& p) const {
    return (p-position).normalized();
}

AABB Sphere::AA_bbox() const {
    return AABB(position-Vector3d(radius, radius, radius), position+Vector3d(radius, radius, radius));
}

Vector3d Sphere::AA_bbox_center() const {
    return position;
}

Vector3d Sphere::barycentric_center() const {
    return position;
}

Sphere::~Sphere() {}

Tri::Tri(Object* i, const Vector3d& a, const Vector3d& b, const Vector3d& c):
    Primitive(i), p0(a), p1(b), p2(c), normalV((p1-p0).cross(p2-p0).normalized()) {}

Vector3d Tri::intersection_point(const Ray& r) {
    Vector3d e1 = p1-p0;
    Vector3d e2 = p2-p0;
    Vector3d h = r.dir.cross(e2);
    double det = h.dot(e1);
    if(det<epsilon&&det>-epsilon) return Vector3d(inf, inf, inf);
    double invDet = 1/det;
    Vector3d s = r.origin-p0;
    double u = h.dot(s)*invDet;
    if(u<0||u>1) return Vector3d(inf, inf, inf);
    Vector3d q = s.cross(e1);
    double v = r.dir.dot(q)*invDet;
    if(v<0||u+v>1) return Vector3d(inf, inf, inf);
    double t = e2.dot(q)*invDet;
    return Vector3d(r.origin+r.dir*t);
}

double Tri::intersection_distance(const Ray& r) {
    Vector3d e1 = p1-p0;
    Vector3d e2 = p2-p0;
    Vector3d h = r.dir.cross(e2);
    double det = h.dot(e1);
    if(det<epsilon&&det>-epsilon) return inf;
    double invDet = 1/det;
    Vector3d s = r.origin-p0;
    double u = h.dot(s)*invDet;
    if(u<0||u>1) return inf;
    Vector3d q = s.cross(e1);
    double v = r.dir.dot(q)*invDet;
    if(v<0||u+v>1) return inf;
    double t = e2.dot(q)*invDet;
    return t;
}

Vector3d Tri::normal() const {
    return normalV;
}

Vector3d Tri::normal(const Vector3d&) const {
    return normalV;
}

AABB Tri::AA_bbox() const {
    return AABB(Vector3d(
                    std::min(p0(0), std::min(p1(0), p2(0))),
                    std::min(p0(1), std::min(p1(1), p2(1))),
                    std::min(p0(2), std::min(p1(2), p2(2)))
                ),
                Vector3d(
                    std::max(p0(0), std::max(p1(0), p2(0))),
                    std::max(p0(1), std::max(p1(1), p2(1))),
                    std::max(p0(2), std::max(p1(2), p2(2)))
                ));
}

Vector3d Tri::AA_bbox_center() const {
    return AA_bbox().get_center();
}

Vector3d Tri::barycentric_center() const {
    return (p0+p1+p2)/3;
}

Tri::~Tri() {}

Quad::Quad(Object* i, const Vector3d& a, const Vector3d& b, const Vector3d& c, const Vector3d& d):
    Primitive(i), p0(a), p1(b), p2(c), p3(d), normalV((p1-p0).cross(p3-p0).normalized()) {}

Vector3d Quad::intersection_point(const Ray& r) {
    Tri a = Tri(nullptr, p0, p1, p2);
    Tri b = Tri(nullptr, p0, p2, p3);
    double a_ = a.intersection_distance(r);
    double b_ = b.intersection_distance(r);
    if(a_<inf&&a_>0) return Vector3d(r.origin+r.dir*a_);
    if(b_<inf&&b_>0) return Vector3d(r.origin+r.dir*b_);
    return Vector3d(inf, inf, inf);
}

double Quad::intersection_distance(const Ray& r) {
    Tri a = Tri(nullptr, p0, p1, p2);
    Tri b = Tri(nullptr, p0, p2, p3);
    double a_ = a.intersection_distance(r);
    double b_ = b.intersection_distance(r);
    if(a_<inf&&a_>0) return a_;
    if(b_<inf&&b_>0) return b_;
    return inf;
}

Vector3d Quad::normal() const {
    return normalV;
}

Vector3d Quad::normal(const Vector3d&) const {
    return normalV;
}

AABB Quad::AA_bbox() const {
    return AABB(Vector3d(
                    std::min(std::min(p0(0), p1(0)), std::min(p2(0), p3(0))),
                    std::min(std::min(p0(1), p1(1)), std::min(p2(1), p3(1))),
                    std::min(std::min(p0(2), p1(2)), std::min(p2(2), p3(2)))
                ),
                Vector3d(
                    std::max(std::max(p0(0), p1(0)), std::max(p2(0), p3(0))),
                    std::max(std::max(p0(1), p1(1)), std::max(p2(1), p3(1))),
                    std::max(std::max(p0(2), p1(2)), std::max(p2(2), p3(2)))
                ));
}

Vector3d Quad::AA_bbox_center() const {
    return AA_bbox().get_center();
}

Vector3d Quad::barycentric_center() const {
    return (p0+p1+p2+p3)/3;
}

Quad::~Quad() {}

AABB::AABB() {}

AABB::AABB(const Vector3d& a, const Vector3d& b):
    v_min(a), v_max(b) {}

bool AABB::intersect(const Ray& r) const {
    Vector3d dif_min = (v_min-r.origin);
    Vector3d dif_max = (v_max-r.origin);
    double t1 = dif_min(0)/r.dir(0);
    double t2 = dif_max(0)/r.dir(0);
    double tmin = std::min(t1, t2);
    double tmax = std::max(t1, t2);
    for(int i = 0; i<3; ++i) {
        t1 = dif_min(i)/r.dir(i);
        t2 = dif_max(i)/r.dir(i);
        tmin = std::max(tmin, std::min(t1, t2));
        tmax = std::min(tmax, std::max(t1, t2));
    }
    return tmax>std::max(tmin, 0.0);
}

int AABB::longest_axis() {
    Vector3d diagonal = v_max-v_min;
    int longest_axis = (diagonal(0)>diagonal(1))
                           ? (diagonal(0)>diagonal(2))
                                 ? 0
                                 : 2
                           : (diagonal(1)>diagonal(2))
                           ? 1
                           : 2;
    return longest_axis;
}

Vector3d AABB::get_center() {
    return (v_min+v_max)/2;
}

AABB::~AABB() {}
