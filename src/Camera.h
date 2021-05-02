//
// Created by Karl Rombauts on 2/5/21.
//

#ifndef STARFOX_CAMERA_H
#define STARFOX_CAMERA_H


#include "Vector3.h"

class Camera {
public:
    double fov;
    Vector3 pos;
    Vector3 lookAt; // Doesn't do anything thing atm

    Camera(): fov(60), pos(0,0,0), lookAt(0,0,0) {};
};


#endif //STARFOX_CAMERA_H
