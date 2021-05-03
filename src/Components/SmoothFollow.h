#ifndef STARFOX_SMOOTHFOLLOW_H
#define STARFOX_SMOOTHFOLLOW_H


#include <ecs/Entity.h>
#include "Vector3.h"

struct SmoothFollow : public Component {
    SmoothFollow(Entity *entity, Vector3 relativeOffset)
            : entity(entity), relativeOffset(relativeOffset) {}

    Entity *entity;
    Vector3 relativeOffset;
};


#endif //STARFOX_SMOOTHFOLLOW_H
