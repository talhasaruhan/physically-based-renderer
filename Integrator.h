#pragma once
#include "Camera.h"
#include <vector>

class Scene;

class Integrator {
public:
    Integrator();

    virtual std::vector<double> render(Scene*) = 0;


    ~Integrator();
};
