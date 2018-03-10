#ifndef PRIMITVES_H
#define PRIMITVES_H

#include <Eigen/Dense>
#include "Material.h"
#include "Primitive.h"
#include "MathUtils.h"

using namespace Eigen;

class Sphere:
    public Primitive {
private:

public:
    Vector3d c_normal;
    Vector3d position;
    double radius;

    Sphere(Object*, double, const Vector3d&);

    Vector3d intersection_point(const Ray&);
    double intersection_distance(const Ray&);
    Vector3d normal() const;
    Vector3d normal(const Vector3d&) const;
    AABB AA_bbox() const;
    Vector3d AA_bbox_center() const;
    Vector3d barycentric_center() const;

    ~Sphere();
};

class Tri:
    public Primitive {
public:
    Vector3d p0;
    Vector3d p1;
    Vector3d p2;
    Vector3d normalV;

    Tri(Object*, const Vector3d&, const Vector3d&, const Vector3d&);

    Vector3d intersection_point(const Ray&);
    double intersection_distance(const Ray&);
    Vector3d normal() const;
    Vector3d normal(const Vector3d&) const;
    AABB AA_bbox() const;
    Vector3d AA_bbox_center() const;
    Vector3d barycentric_center() const;

    ~Tri();
};

class Quad:
    public Primitive {
public:
    Vector3d p0;
    Vector3d p1;
    Vector3d p2;
    Vector3d p3;
    Vector3d normalV;

    Quad(Object*, const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&);

    Vector3d intersection_point(const Ray&);
    double intersection_distance(const Ray&);
    Vector3d normal() const;
    Vector3d normal(const Vector3d&) const;
    AABB AA_bbox() const;
    Vector3d AA_bbox_center() const;
    Vector3d barycentric_center() const;

    ~Quad();
};

class AABB {
public:
    Vector3d v_min;
    Vector3d v_max;

    AABB();
    AABB(const Vector3d&, const Vector3d&);

    bool intersect(const Ray&) const;
    Vector3d get_center();
    int longest_axis();

    ~AABB();
};

#endif
