#ifndef STARFOX_PLANECOLLISION_H
#define STARFOX_PLANECOLLISION_H


#include "Plane.h"

struct PlaneCollision : public Component {
    PlaneCollision(Plane *plane) : plane(plane) {};

    Plane *plane;
};


#endif //STARFOX_PLANECOLLISION_H
