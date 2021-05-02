#ifndef STARFOX_RAYCASTINGSYSTEM_H
#define STARFOX_RAYCASTINGSYSTEM_H


#include <ecs/EntityManager.h>

struct RayCastingSystem {
    void update(EntityManager &entities);
};


#endif //STARFOX_RAYCASTINGSYSTEM_H
