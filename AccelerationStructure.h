#pragma once
#include <atomic>
#include "Ray.h"
#include "Scene.h"

class IntersectionData{
public:
    Vector3d point;
    Vector3d normal;
    double distance;

    IntersectionData(){}
    ~IntersectionData(){}
};

class AccelerationStructure
{
public:
    AccelerationStructure();

    virtual Primitive* search(const Ray&, IntersectionData&) =0;
    virtual std::pair<int, int> info() =0;

    virtual ~AccelerationStructure();
};

class BruteForce: public AccelerationStructure{
public:
    Scene* scene;

    BruteForce(Scene*);

    Primitive* search(const Ray&, IntersectionData&);
    std::pair<int, int> info();

    ~BruteForce();
};

class KDTree: public AccelerationStructure{
public:
    Scene* scene;

    KDTree(Scene*);

    Primitive* search(const Ray&, IntersectionData&);

    ~KDTree();
};

