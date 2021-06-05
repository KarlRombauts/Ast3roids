//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_COLLISIONSYSTEM_H
#define UNTITLED_COLLISIONSYSTEM_H


#include "System.h"

class CollisionSystem: public System {
public:
    void update(EntityManager &entities, double dt) override;

private:
    bool areSpheresIntersecting(Entity *entity1, Entity *entity2) const;

    void createImpacts(Entity *entity1, Entity *entity2) const;

    bool intersectingWithArenaWall(Entity *pEntity);

    void resolveArenaWallCollision(Entity *entity) const;

    void createArenaImpacts(Entity *entity) const;

    Vector3 calculateNewVelocity(Vector3 p1, Vector3 p2, Vector3 v1, Vector3 v2, double m1, double m2) const;

    void wallCollisionDynamicResolution(Entity *entity) const;

    void wallCollisionStaticResolution(Vector3 &position, double radius) const;

    void resolveCircleCircleCollision(Entity *entity1, Entity *entity2);
};


#endif //UNTITLED_COLLISIONSYSTEM_H
