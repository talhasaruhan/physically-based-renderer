#pragma once
#include <cmath>
#include "Integrator.h"
#include "Ray.h"

class PathTracer:
    public Integrator {
public:
    int n_samples, min_depth, max_depth;

    PathTracer();

    std::vector<double> render(Scene*);
    void trace(Ray&, Scene*) const;
    void _trace(Ray&, Scene*, int) const;

    ~PathTracer();
};
