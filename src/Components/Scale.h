#ifndef STARFOX_SCALE_H
#define STARFOX_SCALE_H

#include <ecs/Component.h>
#include <Vector3.h>

struct Scale : public Component {
    explicit Scale(): scale(1, 1, 1) {}

    explicit Scale(double scale): scale(scale, scale, scale) {}

    explicit Scale(Vector3 scale): scale(scale) {}

    Vector3 scale;
};

#endif //STARFOX_SCALE_H
