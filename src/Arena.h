//
// Created by Karl Rombauts on 4/5/21.
//

#ifndef STARFOX_ARENA_H
#define STARFOX_ARENA_H


#include <ecs/Entity.h>

struct Arena {
    double size = 100;
    Entity *leftWall = nullptr;
    Entity *rightWall = nullptr;
    Entity *topWall = nullptr;
    Entity *bottomWall = nullptr;
    Entity *backWall = nullptr;
    Entity *frontWall = nullptr;
};


#endif //STARFOX_ARENA_H
