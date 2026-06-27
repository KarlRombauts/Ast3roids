//
// Created by Karl Rombauts on 4/6/21.
//

#ifndef STARFOX_LOOKAT_H
#define STARFOX_LOOKAT_H


#include <ecs/Entity.h>

struct LookAt : Component {
    LookAt(Entity *target, double roll = 0) : target(target), roll(roll) {}
    Entity *target;
    double roll; // extra rotation (radians) about the view axis, for variety
};


#endif //STARFOX_LOOKAT_H
