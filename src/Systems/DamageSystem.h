//
// Created by Karl Rombauts on 10/4/21.
//

#ifndef UNTITLED_DAMAGESYSTEM_H
#define UNTITLED_DAMAGESYSTEM_H


#include "../ecs/EntityManager.h"

class DamageSystem {
public:
    void update(EntityManager &entities);

    void handleDeath(EntityManager &entities, Entity *entity,
                     Entity *otherEntity) const;
};


#endif //UNTITLED_DAMAGESYSTEM_H
