//
// Created by Karl Rombauts on 16/3/21.
//

#ifndef UNTITLED_FIRINGSYSTEM_H
#define UNTITLED_FIRINGSYSTEM_H


#include "System.h"

class FiringSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;
};


#endif //UNTITLED_FIRINGSYSTEM_H
