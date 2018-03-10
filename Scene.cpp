#include "Scene.h"
#include <algorithm>
#include "lodepng.h"
#include "ctpl_stl.h"

Scene::Scene() {
}

inline double _reinhard(double c, double m){
    return c/(1+c)*(1+c/(m*m));
}

inline double _hejl_burgassDawson(double c){
    double x = std::fmax(0, c-0.004);
    return (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
}

inline double _gamma_correction(double c) {
    if(c<0.0031308)
        return 12.92*c;
    else
        return 1.055*pow(c, 1/2.4)-0.055;
}

inline std::vector<unsigned char> tone_map(const std::vector<double>& hdr_image){
    int sz = hdr_image.size();
    std::vector<unsigned char> ldr_image(sz);

    double m_r = 0, m_g = 0, m_b = 0;
    for(int i = 0; i<sz/4; ++i){
        m_r = std::fmax(m_r, hdr_image[4*i+0]);
        m_g = std::fmax(m_g, hdr_image[4*i+1]);
        m_b = std::fmax(m_b, hdr_image[4*i+2]);
    }
    m_r = std::fmax(m_r, 1.0);
    m_g = std::fmax(m_g, 1.0);
    m_b = std::fmax(m_b, 1.0);

    for(int i = 0; i<sz/4; ++i){
        ldr_image[4*i+0] = 255*_gamma_correction(_reinhard(hdr_image[4*i+0], m_r));
        ldr_image[4*i+1] = 255*_gamma_correction(_reinhard(hdr_image[4*i+1], m_g));
        ldr_image[4*i+2] = 255*_gamma_correction(_reinhard(hdr_image[4*i+2], m_b));

        ldr_image[4*i+3] = 255*hdr_image[4*i+3];
    }

    return ldr_image;
}

void Scene::render() {
    std::vector<double> hdr_image = integrator->render(this);
    std::vector<unsigned char> ldr_image = tone_map(hdr_image);
    lodepng::encode(out_file, ldr_image, width, height);
}


Scene::~Scene() {}
