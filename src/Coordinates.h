#ifndef UNTITLED_COORDINATES_H
#define UNTITLED_COORDINATES_H


#include "Vector3.h"

struct CoordinateSpace {
    double minX;
    double maxX;
    double minY;
    double maxY;

    double distanceToCorner();
};


#endif //UNTITLED_COORDINATES_H
