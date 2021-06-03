//
// Created by Karl Rombauts on 4/6/21.
//

#ifndef STARFOX_LOOKATSYSTEM_H
#define STARFOX_LOOKATSYSTEM_H


#include <ecs/EntityManager.h>

class LookAtSystem {

public:
    void update(EntityManager &entities);
};


#endif //STARFOX_LOOKATSYSTEM_H
