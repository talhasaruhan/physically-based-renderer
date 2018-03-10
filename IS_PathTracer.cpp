#include "IS_PathTracer.h"
#include "Scene.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include <chrono>
#include <mutex>
#include "ctpl_stl.h"

/*
bug:
multiple threads introduce noise!

* happens with all shaders
* when num threads is set to 1 noise disappears
* mutexing any of the samples removes noise
* disabling light sampling when doing direct lighting greatly increases noise, both in 1 thread case and multithread case

*/

const double PI = 3.14;
const double epsilon = 0.001;
const double inf = std::numeric_limits<double>::infinity();

std::mutex image_mutex;
std::mutex t_mutex;

IS_PathTracer::IS_PathTracer() {}

// branching tracer for primary camera rays
Primitive* IS_PathTracer::trace(Ray& r, Scene* scene) const {
    IntersectionData intersection_data;
    Primitive* intrsct_clst_obj = acceleration_structure->search(r, intersection_data);
    Vector3d intrsct_obj_point = intersection_data.point;
    Vector3d intrsct_obj_normal = intersection_data.normal;
    double intrsct_obj_distance = intersection_data.distance;

    Color4dRGB obj_indirect(0.0), obj_direct(0.0), direct_light(0.0);
    const Ray initial_ray = r;

    Light* intrsct_clst_light = nullptr;
    double intrsct_light_distance = 0;
    for(Light* light : scene->lights) {
        intrsct_light_distance = light->intersection_distance(r);
        if(intrsct_light_distance<inf&&(!intrsct_clst_obj||intrsct_light_distance<intrsct_obj_distance)) {
            direct_light = direct_light+light->color;
            intrsct_clst_light = light;
        }
    }

    if(intrsct_clst_light) {
        r.color = r.color*intrsct_clst_light->color;
        return nullptr;
    }

    if(!intrsct_clst_obj) {
        r.color = r.color*scene->ambient;
        return nullptr;
    }
    
    int n_light_samples = intrsct_clst_obj->parent->direct_sample_lights ? n_primary_direct_samples : 0,
        n_surface_samples = n_primary_direct_samples;

    if(intrsct_clst_obj->parent->direct_sample) {
        for(Light* light : scene->lights) {
            for(int i = 0; i<n_light_samples; ++i) {
                Vector3d p_light = light->sample_area(intrsct_obj_point);
                Vector3d light_dir = (p_light-intrsct_obj_point);
                double light_dist = light_dir.norm();
                light_dir /= light_dist;
                if(light_dir.dot(intrsct_obj_normal)>0) {
                    Color4dRGB color = intrsct_clst_obj->parent->material->explicit_eval(
                        r.dir, light_dir, intrsct_obj_normal);
                    Ray r_light = Ray(intrsct_obj_point+light_dir*0.01, light_dir, color);
                    IntersectionData direct_pass;
                    Primitive* o = acceleration_structure->search(r_light, direct_pass);
                    double surface_pdf = intrsct_clst_obj->parent->material->pdf(-initial_ray.dir, light_dir, intrsct_obj_normal);
                    double light_pdf = light->pdf(r_light.origin, p_light);
                    if(light_dist<direct_pass.distance) {
                        obj_direct = obj_direct+r.color*color*light->direct_light_sample(r_light.origin, p_light)*light_pdf/(n_light_samples*light_pdf+n_surface_samples*surface_pdf);
                    }
                }
            }
        }

        for(int i = 0; i<n_surface_samples; ++i) {
            intrsct_clst_obj->parent->material->eval(r, intrsct_obj_point, intrsct_obj_normal);

            IntersectionData direct_pass;
            Primitive* o = acceleration_structure->search(r, direct_pass);
            const double shadow_ray_intrsct_obj_dist = direct_pass.distance;
            double shadow_ray_intrsct_light_dist = inf;
            Vector3d shadow_ray_intrsct_light_point;

            for(Light* light : scene->lights) {
                shadow_ray_intrsct_light_point = light->intersection_point(r);
                shadow_ray_intrsct_light_dist = light->intersection_distance(r);
                double surface_pdf = intrsct_clst_obj->parent->material->pdf(-initial_ray.dir, r.dir,intrsct_obj_normal);
                double light_pdf = light->pdf(intrsct_obj_point, shadow_ray_intrsct_light_point);
                if(shadow_ray_intrsct_light_dist<shadow_ray_intrsct_obj_dist && 
                    light->normal(shadow_ray_intrsct_light_point).dot(r.dir)<0) {
                    obj_direct = obj_direct+r.color*light->color*surface_pdf/(n_light_samples*light_pdf+n_surface_samples*surface_pdf);
                }
            }
            r = initial_ray;
        }
    }

    for(int i = 0; i<n_indirect_samples; ++i) {
        intrsct_clst_obj->parent->material->eval(r, intrsct_obj_point, intrsct_obj_normal);

        if(!r.terminated) {
            _trace(r, scene, 1);
        }

        obj_indirect = obj_indirect+r.color;
        r = initial_ray;
    }

    if(n_indirect_samples>0)
        obj_indirect = obj_indirect/n_indirect_samples;

    r.color = obj_direct+obj_indirect;

    return intrsct_clst_obj;
}

// non branching tracer for secondary rays
void IS_PathTracer::_trace(Ray& r, Scene* scene, int depth) const {
    IntersectionData intersection_data;
    Primitive* intrsct_clst_obj = acceleration_structure->search(r, intersection_data);
    Vector3d intrsct_obj_point = intersection_data.point;
    Vector3d intrsct_obj_normal = intersection_data.normal;
    double intrsct_obj_distance = intersection_data.distance;

    if(!intrsct_clst_obj) {
        r.color = r.color*scene->ambient;
        return;
    }

    Color4dRGB obj_direct(0.0);
    const Ray initial_ray = r;
    int n_light_samples = intrsct_clst_obj->parent->direct_sample_lights ? n_secondary_direct_samples : 0,
        n_surface_samples = n_secondary_direct_samples;
    
    if(intrsct_clst_obj->parent->direct_sample) {
        for(Light* light:scene->lights) {
            for(int i = 0; i<n_light_samples; ++i) {
                Vector3d p_light = light->sample_area(intrsct_obj_point);
                Vector3d light_dir = (p_light-intrsct_obj_point);
                double light_dist = light_dir.norm();
                light_dir /= light_dist;
                if(light_dir.dot(intrsct_obj_normal)>0) {
                    Color4dRGB color = intrsct_clst_obj->parent->material->explicit_eval(
                        r.dir, light_dir, intrsct_obj_normal);
                    Ray r_light = Ray(intrsct_obj_point+light_dir*0.01, light_dir, color);
                    IntersectionData direct_pass;
                    Primitive* o = acceleration_structure->search(r_light, direct_pass);
                    double surface_pdf = intrsct_clst_obj->parent->material->pdf(-initial_ray.dir, light_dir,
                                                                                    intrsct_obj_normal);
                    double light_pdf = light->pdf(r_light.origin, p_light);
                    if(light_dist<direct_pass.distance) {
                        obj_direct = obj_direct+r.color*color*light->direct_light_sample(r_light.origin, p_light)*light_pdf/(n_light_samples*light_pdf+n_surface_samples*surface_pdf);
                    }
                }
            }
        }

        for(int i = 0; i<n_surface_samples; ++i) {
            intrsct_clst_obj->parent->material->eval(r, intrsct_obj_point, intrsct_obj_normal);
            IntersectionData direct_pass;
            Primitive* o = acceleration_structure->search(r, direct_pass);
            double shadow_ray_intrsct_obj_dist = direct_pass.distance, shadow_ray_intrsct_light_dist = inf;
            Vector3d shadow_ray_intrsct_light_point;

            for(Light* light:scene->lights) {
                shadow_ray_intrsct_light_point = light->intersection_point(r);
                shadow_ray_intrsct_light_dist = light->intersection_distance(r);
                double surface_pdf = intrsct_clst_obj->parent->material->pdf(-initial_ray.dir, r.dir,
                                                                                intrsct_obj_normal);
                double light_pdf = light->pdf(intrsct_obj_point, shadow_ray_intrsct_light_point);
                if(shadow_ray_intrsct_light_dist<shadow_ray_intrsct_obj_dist&&light
                    ->normal(shadow_ray_intrsct_light_point).
                    dot(r.dir)<0) {
                    obj_direct = obj_direct+r.color*light->color*surface_pdf/(n_light_samples*light_pdf+n_surface_samples*surface_pdf);
                }
            }
            r = initial_ray;
        }
    }
    intrsct_clst_obj->parent->material->eval(r, intrsct_obj_point, intrsct_obj_normal);

    if(!r.terminated&&depth<max_depth) {
        _trace(r, scene, depth+1);
    }
    r.color = obj_direct+r.color;
}

void _render_tile(int id, IS_PathTracer* pt, Scene* scene, double* image, int y1, int y2, int x1, int x2, std::function<int(void)> queued_count){
    Camera* camera = scene->camera;
    unsigned width = scene->width, height = scene->height;
    Vector3d camspc_screen_pos = Vector3d(0, 0, abs(width/(2*tan(camera->horitzontal_fov/2))));
    Vector3d wrldspc_screen_pos = camera->rotation*camspc_screen_pos;
    Vector3d wrldspc_scrn_x = camera->rotation.col(0), 
        wrldspc_scrn_y = camera->rotation.col(1), 
        wrldspc_scrn_z = camera->rotation.col(2);

    if(y2>width||x2>height||y1<0||x1<0){ printf("soemthing is wrong! %d %d %d %d\n", x1, x2, y1, y2); return;  }

    //std::vector<double> tile((y2-y1)*(x2-x1)*4);
    //int p=0;

    for(int i = y1; i<y2; ++i) {
        for(int j = x1; j<x2; ++j) {
            int px = j*width+i;
            double scrnspc_x = (int)(height/2)-j;
            double scrnspc_y = i-(int)(width/2);
            Vector3d wrldspc_px_pos = wrldspc_screen_pos+wrldspc_scrn_x*scrnspc_x+wrldspc_scrn_y*scrnspc_y;
            Ray ray = Ray(camera->position, wrldspc_px_pos.normalized(), Color4dRGB(1.0));
            Primitive* intrsct_clst_obj = pt->trace(ray, scene);

            if(pt->msaa_level!=MSAA_Level::none) {
                int K;
                const double* jitter_matrix = nullptr;
                if(pt->msaa_level==MSAA_Level::msaa_4x) {
                    K = 4;
                    jitter_matrix = MSAA_jitter_4x;
                } else if(pt->msaa_level==MSAA_Level::msaa_8x) {
                    K = 8;
                    jitter_matrix = MSAA_jitter_8x;
                }

                int overlapping_subsamples = 0;
                Color4dRGB subsample_color(0.0);

                for(int k = 0; k<K; ++k) {
                    scrnspc_x = (int)(height/2)-j+jitter_matrix[2*k];
                    scrnspc_y = i-(int)(width/2)+jitter_matrix[2*k+1];
                    wrldspc_px_pos = wrldspc_screen_pos+wrldspc_scrn_x*scrnspc_x+wrldspc_scrn_y*scrnspc_y;
                    Ray subsample_ray = Ray(camera->position, wrldspc_px_pos.normalized(), Color4dRGB(1.0));

                    IntersectionData intersection_data;
                    Primitive* o = pt->acceleration_structure->search(subsample_ray, intersection_data);
                    if(intrsct_clst_obj==o) {
                        overlapping_subsamples++;
                    } else {
                        pt->trace(subsample_ray, scene);
                        subsample_color = subsample_color+subsample_ray.color;
                    }
                }

                ray.color = (ray.color*overlapping_subsamples+subsample_color)/K;
            }
            
            image[px * 4] = ray.color.r;
            image[px * 4 + 1] = ray.color.g;
            image[px * 4 + 2] = ray.color.b;
            image[px * 4 + 3] = 1.0;
        }
    }

    int queued_tiles = queued_count();
}

int job_queue_size(ctpl::thread_pool* tp){
    return tp->queued_count();
}

std::vector<double> IS_PathTracer::render(Scene* scene) {
    Camera* camera = scene->camera;
    unsigned width = scene->width, height = scene->height;

    // build world and screen spaces
    // depth of virtual screen (slice of the visual field pyramid) s.t resolution matches the final image
    Vector3d camspc_screen_pos = Vector3d(0, 0, abs(width/(2*tan(camera->horitzontal_fov/2))));
    Vector3d wrldspc_screen_pos = camera->rotation*camspc_screen_pos;
    Vector3d wrldspc_scrn_x = camera->rotation.col(0), 
        wrldspc_scrn_y = camera->rotation.col(1), 
        wrldspc_scrn_z = camera->rotation.col(2);

    std::vector<double> image(width*height*4);
    
    // build acceleration structure
    acceleration_structure = new BruteForce(scene);

    // render tiles
    ctpl::thread_pool thread_pool(NUM_THREADS);

    auto start = std::chrono::steady_clock::now();

    int step_x, step_y;
    for(int i = 0; i<(int)(ceil((double)(width)/tile_size)); ++i){
        step_y = (i*tile_size+tile_size < width) ? tile_size : width-i*tile_size;
        for(int j = 0; j<(int)(ceil((double)(height)/tile_size)); ++j){
            step_x = (j*tile_size+tile_size < height) ? tile_size : height-j*tile_size;
            std::function<int(void)> queued_count = std::bind(job_queue_size, &thread_pool);
            thread_pool.push(
                _render_tile, const_cast<IS_PathTracer*>(this), scene, &image[0], i*tile_size, i*tile_size+step_y, j*tile_size, j*tile_size+step_x, queued_count
            );
        }
    }
    thread_pool.stop(true);
    
    auto end = std::chrono::steady_clock::now();
    std::cout<<std::chrono::duration<double, std::ratio<1, 1>>(end-start).count()<<std::endl;
    
    std::pair<int, int> info = acceleration_structure->info();
    std::cout<<info.first<<", "<<info.second<<std::endl;

    return image;
}

IS_PathTracer::~IS_PathTracer() {}
