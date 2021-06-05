//
// Created by Karl Rombauts on 5/6/21.
//

#ifndef STARFOX_BULLETFACTORY_H
#define STARFOX_BULLETFACTORY_H


#include <Vector3.h>
#include <ecs/EntityManager.h>

class BulletFactory {
public:
    static Entity * create(EntityManager &entities, Vector3 position, Vector3 velocity);

    static void setKinematics(Entity *bullet, Vector3 &velocity);

    static void setGeometry(Entity *bullet);

    static void setTransformations(Entity *bullet, Vector3 &position);

    static void setCollisions(Entity *bullet);
};


#endif //STARFOX_BULLETFACTORY_H
