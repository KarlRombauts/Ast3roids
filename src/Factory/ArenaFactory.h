#ifndef STARFOX_ARENAFACTORY_H
#define STARFOX_ARENAFACTORY_H

#include <ecs/Entity.h>
#include <ecs/EntityManager.h>
#include "Arena.h"

enum class ArenaWall {
    TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK
};

class ArenaFactory {
public:
    static Arena & create(EntityManager &entities);

private:
    static Entity *createWall(EntityManager &entities, ArenaWall side);

    static Entity *createGridPlane(Entity *plane, Vector3 bottomLeft,
                            Vector3 bottomRight, Vector3 topRight,
                            Vector3 topLeft);
};


#endif //STARFOX_ARENAFACTORY_H
