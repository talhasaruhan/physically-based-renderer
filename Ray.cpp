#include "Ray.h"

Ray::Ray(const Vector3d& o, const Vector3d& d):
    origin(o), dir(d), color(Color4dRGB(0)), terminated(false) {}

Ray::Ray(const Vector3d& o, const Vector3d& d, const Color4dRGB& c) :
    origin(o), dir(d), color(c), terminated(false) {}

Ray::~Ray() {}
