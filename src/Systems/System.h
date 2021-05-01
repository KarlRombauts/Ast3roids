//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_SYSTEM_H
#define UNTITLED_SYSTEM_H

#include "../ecs/EntityManager.h"

class System {
public:
    virtual void update(EntityManager &entities, double dt) = 0;
};
#endif //UNTITLED_SYSTEM_H
