#pragma once

#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Primitives.h"
#include "Integrator.h"
#include "Light.h"
#include "Color.h"

class Scene {
public:

    //Config* configuration;
    Camera* camera;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    std::vector<Primitive*> primitives;
    std::vector<Material*> materials;
    Color4dRGB ambient;    
    int width, height, time_limit;
    bool halt, incremental_save;
    std::string out_file;
    Integrator* integrator;

    Scene();

    void render();

    ~Scene();
};
