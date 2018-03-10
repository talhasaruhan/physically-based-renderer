#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <Eigen/Dense>
#include "Ray.h"
#include "Entity.h"

class AABB;

class Material;

using namespace Eigen;

class Object {
public:
    int id;
    bool direct_sample;
    bool direct_sample_lights;
    Material* material;
    std::vector<int> primitives;

    Object(int, Material*);
    ~Object();
};

#endif
