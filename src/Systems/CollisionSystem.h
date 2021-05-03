//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_COLLISIONSYSTEM_H
#define UNTITLED_COLLISIONSYSTEM_H


#include "System.h"

class CollisionSystem: public System {
public:
    void update(EntityManager &entities, double dt) override;

public:
    void resolveCircleCircleCollision(EntityManager &entities, Entity *entity1,
                                      Entity *entity2);

    bool areSpheresIntersecting(Entity *entity1, Entity *entity2) const;

    const bool areCircleAndLineIntersecting(Entity *circle, Entity *line) const;

    void resolveCircleLineCollision(Entity *circle, Entity *line) const;

    void createImpacts(Entity *entity1, Entity *entity2) const;

    bool intersectingWithArenaWall(Entity *pEntity);

    void handleArenaWallCollision(Entity *entity) const;
};


#endif //UNTITLED_COLLISIONSYSTEM_H
