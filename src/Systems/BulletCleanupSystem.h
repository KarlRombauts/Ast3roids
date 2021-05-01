//
// Created by Karl Rombauts on 10/4/21.
//

#ifndef UNTITLED_BULLETCLEANUPSYSTEM_H
#define UNTITLED_BULLETCLEANUPSYSTEM_H


#include "../ecs/EntityManager.h"
#include "System.h"

class BulletCleanupSystem: public System {
public:
    void update(EntityManager &entities, double dt) override;
};


#endif //UNTITLED_BULLETCLEANUPSYSTEM_H
