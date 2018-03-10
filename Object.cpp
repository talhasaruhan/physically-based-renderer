#include "Object.h"

Object::Object(int i, Material* m):
    id(i), material(m), direct_sample(false), direct_sample_lights(false) {}

Object::~Object() {}
