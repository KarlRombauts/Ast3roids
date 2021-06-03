//
// Created by Karl Rombauts on 4/6/21.
//

#ifndef STARFOX_LOOKAT_H
#define STARFOX_LOOKAT_H


#include <ecs/Entity.h>

struct LookAt : Component {
    LookAt(Entity *target) : target(target) {}
    Entity *target;
};


#endif //STARFOX_LOOKAT_H
