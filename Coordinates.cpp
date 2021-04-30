#include "Coordinates.h"
#include "Vec3.h"

double CoordinateSpace::distanceToCorner() {
    return Vec3(maxX, maxY).magnitude();
}

