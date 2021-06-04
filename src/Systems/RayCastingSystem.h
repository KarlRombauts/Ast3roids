#ifndef STARFOX_RAYCASTINGSYSTEM_H
#define STARFOX_RAYCASTINGSYSTEM_H


#include <ecs/EntityManager.h>

struct MouseLookSystem {
    void update(EntityManager &entities, double dt);
};


#endif //STARFOX_RAYCASTINGSYSTEM_H
