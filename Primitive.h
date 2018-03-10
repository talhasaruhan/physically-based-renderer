#pragma once

#include <string>
#include <Eigen/Dense>
#include "Ray.h"
#include "Object.h"

class AABB;

class Material;

using namespace Eigen;

class Primitive {
public:
    Object* parent;

    Primitive(Object*);

    virtual Vector3d intersection_point(const Ray&) = 0;
    virtual double intersection_distance(const Ray&) = 0;
    virtual Vector3d normal() const = 0; // cached
    virtual Vector3d normal(const Vector3d&) const = 0;
    virtual AABB AA_bbox() const = 0;
    virtual Vector3d AA_bbox_center() const = 0;
    virtual Vector3d barycentric_center() const = 0;
    ~Primitive();
};
