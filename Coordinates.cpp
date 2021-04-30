#include "Coordinates.h"
#include "Vector3.h"

double CoordinateSpace::distanceToCorner() {
    return Vector3(maxX, maxY).magnitude();
}

