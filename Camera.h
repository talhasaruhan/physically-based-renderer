#pragma once
#include <Eigen/Dense>

using namespace Eigen;

class Camera {
public:
    Vector3d position;
    Matrix3d rotation;
    double horitzontal_fov;    
    Vector3d wrldspc_scrn_x, wrldspc_scrn_y, wrldspc_scrn_z;

    Camera(Vector3d pos, Matrix3d rot, double hfov = 60);

    ~Camera();
};
