#ifndef STARFOX_PLANECOLLISION_H
#define STARFOX_PLANECOLLISION_H


#include "Plane.h"

class PlaneCollision : public Component {
    PlaneCollision(Plane *plane) : plane(plane) {};

    Plane *plane;
};


#endif //STARFOX_PLANECOLLISION_H
