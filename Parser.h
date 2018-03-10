#ifndef PARSER_H
#define PARSER_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <pugixml.hpp>
#include "Color.h"
#include "Material.h"
#include "Primitives.h"
#include "Scene.h"
#include "Integrator.h"
#include "PathTracer.h"
#include "IS_PathTracer.h"
#include "Light.h"
#include "Diffuse.h"
#include "Emissive.h"
#include "GGX.h"
#include "Reflective.h"
#include "Refractive.h"
#include <map>

namespace Parser {
    Scene* parse(int argc, char* argv[]);
    void configuration_parser(char* filename, Scene*);
    void scene_parser(char* filename, Scene*);
}

#endif
