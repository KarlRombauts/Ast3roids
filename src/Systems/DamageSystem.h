//
// Created by Karl Rombauts on 10/4/21.
//

#ifndef UNTITLED_DAMAGESYSTEM_H
#define UNTITLED_DAMAGESYSTEM_H


#include "../ecs/EntityManager.h"

class DamageSystem {
public:
    void update(EntityManager &entities);


private:
    void splitAsteroid(EntityManager &entities, Entity *asteroid, const Vector3 &splitDir);

    void handleDeath(EntityManager &entities, Entity *entity, Entity *otherEntity);

    void createExplosion(EntityManager &entities, Entity *asteroid);

    Vector3 calculateSplitDirection(Entity *impactObject) const;
};


#endif //UNTITLED_DAMAGESYSTEM_H
