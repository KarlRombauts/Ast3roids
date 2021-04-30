#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "../ecs/Component.h"
#include "../Vec3.h"

struct Transform : public Component {
    Transform() : position(Vec3(0,0)), rotation(0), scale(Vec3(1,1)) {}
    Transform(Vec3 position, double rotation, Vec3 scale)
            : position(position), rotation(rotation), scale(scale) {}

    Vec3 position;
    Vec3 scale;
    double rotation;
};

#endif //UNTITLED_TRANSFORM_H
