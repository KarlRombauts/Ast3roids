//
// Created by Karl Rombauts on 5/6/21.
//

#ifndef STARFOX_SPACESHIPFACTORY_H
#define STARFOX_SPACESHIPFACTORY_H


#include <ecs/EntityManager.h>

class SpaceShipFactory {
public:
    static Entity *create(EntityManager &entities, const Position &position);

private:
    static void setBoundingCircle(EntityManager &entities, Entity *spaceShip);

    static void setCollisions(Entity *spaceShip);

    static void setGeometry(Entity *spaceShip);

    static void setTransformations(Entity *spaceShip, const Position &position);

    static void setKinematics(Entity *spaceShip);
};


#endif //STARFOX_SPACESHIPFACTORY_H
