#pragma once
#include <cmath>
#include "Integrator.h"
#include "Ray.h"
#include "Object.h"
#include "Primitive.h"
#include "AccelerationStructure.h"

enum MSAA_Level {
    none,
    msaa_4x,
    msaa_8x
};

class IS_PathTracer:
    public Integrator {
public:

    int NUM_THREADS;
    int n_indirect_samples, n_primary_direct_samples, n_secondary_direct_samples, min_depth, max_depth, tile_size;
    MSAA_Level msaa_level;

    IS_PathTracer();

    // search utility, searches space for intersections

    // trace first ray
    Primitive* trace(Ray&, Scene*) const;

    // recursively trace further
    void _trace(Ray&, Scene*, int) const;

    // renders the image
    std::vector<double> render(Scene*);

    AccelerationStructure* acceleration_structure;

    ~IS_PathTracer();
private:
};

const double MSAA_jitter_4x[4*2] = {
    3.0/4.0, -1.0/4.0,
    1.0/4.0, 3.0/3.0,
    -1.0/4.0, -3.0/4.0,
    -3.0/4.0, 1.0/4.0
};

const double MSAA_jitter_8x[8*2] = {
    3.0/4.0, 1.0/4.0,
    -3.0/4.0, -1.0/3.0,
    -1.0/4.0, 5.0/4.0,
    5.0/4.0, -3.0/4.0,
    -5.0/4.0, -5.0/4.0,
    1.0/4.0, -7.0/4.0,
    -7.0/4.0, 3.0/4.0,
    7.0/4.0, 7.0/4.0
};
