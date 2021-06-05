//
// Created by Karl Rombauts on 5/6/21.
//

#ifndef STARFOX_CAMERAFACTORY_H
#define STARFOX_CAMERAFACTORY_H


#include <ecs/EntityManager.h>

class CameraFactory {
public:
    static Entity *create(EntityManager &entities, Entity *smoothFollowTarget);
};


#endif //STARFOX_CAMERAFACTORY_H
