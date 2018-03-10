#ifndef RAY_H
#define RAY_H

#include <Eigen/Dense>
#include "Color.h"

using namespace Eigen;

class Ray {
public:
    bool terminated;
    Vector3d origin, dir;
    Color4dRGB color;
    Ray(const Vector3d& o, const Vector3d& d);
    Ray(const Vector3d& o, const Vector3d& d, const Color4dRGB& c);
    ~Ray();
};

#endif
