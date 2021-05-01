//
// Created by Karl Rombauts on 10/4/21.
//

#ifndef UNTITLED_IMPACTCLEANUPSYSTEM_H
#define UNTITLED_IMPACTCLEANUPSYSTEM_H


#include "System.h"

class ImpactCleanupSystem : public System {

public:
    void update(EntityManager &entities, double dt) override;
};


#endif //UNTITLED_IMPACTCLEANUPSYSTEM_H
