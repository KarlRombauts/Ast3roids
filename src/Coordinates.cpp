#include "Coordinates.h"
#include "Vector3.h"
#include "Vector2.h"

double CoordinateSpace::distanceToCorner() {
    return Vector2(maxX, maxY).magnitude();
}

