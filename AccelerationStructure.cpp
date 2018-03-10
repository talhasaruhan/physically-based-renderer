#include "AccelerationStructure.h"
#include <fstream>

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

std::atomic<int> check{0};
std::atomic<int> hit{0};

AccelerationStructure::AccelerationStructure() {}

AccelerationStructure::~AccelerationStructure() {}

BruteForce::BruteForce(Scene* _scene): scene(_scene) {}

Primitive* BruteForce::search(const Ray& r, IntersectionData& id) {
    double min_dist = std::numeric_limits<double>::infinity(), dist;
    Primitive* clst_obj = nullptr;
    check++;
    for(Primitive* obj : scene->primitives) {
        dist = obj->intersection_distance(r);
        if(dist>0&&dist<min_dist) {
            min_dist = dist;
            clst_obj = obj;
        }
        if(dist>0&&dist<inf){
            hit++;
        }
    }

    if(clst_obj) {
        id.point = clst_obj->intersection_point(r);
        id.normal = clst_obj->normal(id.point);
        id.distance = min_dist;
        return clst_obj;
    } else {
        id.point = Vector3d::Identity();
        id.normal = Vector3d::Identity();
        id.distance = min_dist;
        return clst_obj;
    }
}

std::pair<int, int> BruteForce::info() {
    return std::make_pair(int(check), int(hit));
}

BruteForce::~BruteForce() {}

KDTree::KDTree(Scene* _scene): scene(_scene) {}

Primitive* KDTree::search(const Ray& r, IntersectionData& id) {
    return nullptr;
}

KDTree::~KDTree() {}
