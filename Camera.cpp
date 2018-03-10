#include "Camera.h"

Camera::Camera(Vector3d pos, Matrix3d rot, double hfov):
    position(pos), rotation(rot), horitzontal_fov(hfov) {
    Vector3d wrldspc_scrn_x = rotation.col(0), 
        wrldspc_scrn_y = rotation.col(1), 
        wrldspc_scrn_z = rotation.col(2);
}

Camera::~Camera() {}
