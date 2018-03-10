#include "PathTracer.h"
#include "Scene.h"
#include <iostream>
#include "lodepng.h"

PathTracer::PathTracer() {}

void PathTracer::trace(Ray& r, Scene* scene) const {
    double min_dist = std::numeric_limits<double>::infinity(), dist;
    Primitive* clst_obj = nullptr;
    for(Primitive* obj : scene->primitives) {
        dist = obj->intersection_distance(r);
        if(dist>0&&dist<min_dist) {
            min_dist = dist;
            clst_obj = obj;
        }
    }

    if(!clst_obj) {
        r.color = r.color*scene->ambient;
        return;
    }

    if(max_depth<1) {
        clst_obj->parent->material->eval(r, clst_obj->intersection_point(r),
                                         clst_obj->normal(clst_obj->intersection_point(r)));
        return;
    }

    Color4dRGB clr(0.0);
    Ray initial_ray = r;

    for(int i = 0; i<n_samples; ++i) {
        clst_obj->parent->material->eval(r, clst_obj->intersection_point(r),
                                         clst_obj->normal(clst_obj->intersection_point(r)));

        if(!r.terminated) {
            _trace(r, scene, 1);
        }

        clr = clr+r.color;
        r = initial_ray;
    }

    r.color = clr/n_samples;

}

void PathTracer::_trace(Ray& r, Scene* scene, int depth) const {
    double min_dist = std::numeric_limits<double>::infinity(), dist;
    Primitive* clst_obj = nullptr;
    for(Primitive* obj : scene->primitives) {
        dist = obj->intersection_distance(r);
        if(dist>0&&dist<min_dist) {
            min_dist = dist;
            clst_obj = obj;
        }
    }

    if(!clst_obj) {
        r.color = r.color*scene->ambient;
        return;
    }

    clst_obj->parent->material->eval(r, clst_obj->intersection_point(r),
                                     clst_obj->normal(clst_obj->intersection_point(r)));
    if(!r.terminated&&depth<max_depth) {
        _trace(r, scene, depth+1);
    }
}

std::vector<double> PathTracer::render(Scene* scene) {
    srand(time(NULL));
    Camera* camera = scene->camera;
    unsigned width = scene->width, height = scene->height;

    // depth of virtual screen (slice of the visual field pyramid) s.t resolution matches the final image
    Vector3d camspc_screen_pos = Vector3d(0, 0, abs(width/(2*tan(camera->horitzontal_fov/2))));
    Vector3d wrldspc_screen_pos = camera->rotation*camspc_screen_pos;
    Vector3d wrldspc_scrn_x = camera->rotation.col(0),
        wrldspc_scrn_y = camera->rotation.col(1), 
        wrldspc_scrn_z = camera->rotation.col(2);

    std::vector<double> image(width*height*4);

    for(int i = 0; i<width; ++i) {
        for(int j = 0; j<height; ++j) {
            int px = j*width+i;
            int scrnspc_x = height/2-j;
            int scrnspc_y = i-width/2;
            Vector3d wrldspc_px_pos = wrldspc_screen_pos+wrldspc_scrn_x*scrnspc_x+wrldspc_scrn_y*scrnspc_y;
            Ray ray = Ray(camera->position, wrldspc_px_pos.normalized(), Color4dRGB(1.0));
            trace(ray, scene);
            image[px*4] = ray.color.r;
            image[px*4+1] = ray.color.g;
            image[px*4+2] = ray.color.b;
            image[px*4+3] = 1.0;
        }
    }

    return image;
}


PathTracer::~PathTracer() {}
