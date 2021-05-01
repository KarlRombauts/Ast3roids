//
// Created by Karl Rombauts on 18/4/21.
//

#ifndef UNTITLED_BLACKHOLESYSTEM_H
#define UNTITLED_BLACKHOLESYSTEM_H


#include "System.h"

class BlackHoleSystem: public System {
public:
    void update(EntityManager &entities, double dt) override;

};


#endif //UNTITLED_BLACKHOLESYSTEM_H
