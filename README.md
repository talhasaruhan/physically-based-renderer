# physically-based-renderer

## Monte-Carlo Path Tracer
Currently in the state of development

### Features:
* Multiple importance sampling
* Threaded (C++ 11 threads)
* Custom scene description model (obj file importer is also available)
* Modular, OO approach

### Shaders:
* Emissive
* Diffuse
* Cook-Torrence with GGX microfacet
* Ideal refractive
* Ideal reflective

### Notes:
The renderer is currently far from optimal, but once I get it working the way I want it to, shader and cache locality optimizations can be made.

### Known Bugs:
* A bug with threading where LF noise appears as black dots when reflective materials and multiple threads are used

*Requires Eigen, lodepng and pugixml*