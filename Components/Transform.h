#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "../ecs/Component.h"
#include "../Vector3.h"

struct Transform : public Component {
    Transform() : position(Vector3(0, 0)), rotation(0), scale(Vector3(1, 1)) {}
    Transform(Vector3 position, double rotation, Vector3 scale)
            : position(position), rotation(rotation), scale(scale) {}

    Vector3 position;
    Vector3 scale;
    double rotation;
};

#endif //UNTITLED_TRANSFORM_H
