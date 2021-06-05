#ifndef STARFOX_MOUSELOOKSYSTEM_H
#define STARFOX_MOUSELOOKSYSTEM_H


#include <ecs/EntityManager.h>

struct MouseLookSystem {
    void update(EntityManager &entities, double dt);
};


#endif //STARFOX_MOUSELOOKSYSTEM_H
