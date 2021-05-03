//
// Created by Karl Rombauts on 3/5/21.
//

#ifndef STARFOX_PLANE_H
#define STARFOX_PLANE_H


#include <Vector3.h>
#include <ecs/Component.h>

struct Plane : public Component {
    Plane(Vector3 bottomLeft, Vector3 bottomRight, Vector3 topRight, Vector3 topLeft) :
          bottomLeft(bottomLeft), bottomRight(bottomRight), topRight(topRight), topLeft(topLeft) {}

    Vector3 bottomLeft;
    Vector3 bottomRight;
    Vector3 topRight;
    Vector3 topLeft;
};


#endif //STARFOX_PLANE_H
