//
// Created by Karl Rombauts on 3/5/21.
//

#ifndef STARFOX_CAMERA_H
#define STARFOX_CAMERA_H


#include <ecs/Component.h>

struct Camera : public Component {
    Camera(double fov, double aspectRatio, double nearClip, double farClip) :
            fov(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip) {}

    double fov;
    double aspectRatio;
    double nearClip;
    double farClip;
    bool active = false;
};


#endif //STARFOX_CAMERA_H
