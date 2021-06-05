//
// Created by Karl Rombauts on 5/6/21.
//

#ifndef STARFOX_ASTEROIDFACTORY_H
#define STARFOX_ASTEROIDFACTORY_H


#include "ecs/EntityManager.h"

class AsteroidFactory {

public:
    static Entity *create(EntityManager &entities, double radius);

    static double setTransformations(Entity *asteroid, double radius);

    static int getSubdivisions(double radius);

    static void setGeometry(Entity *asteroid, double radius);

    static void setKinematics(Entity *asteroid, double radius);

    static void setCollisions(Entity *asteroid, double radius);
};


#endif //STARFOX_ASTEROIDFACTORY_H
