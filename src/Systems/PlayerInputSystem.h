//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_PLAYERINPUTSYSTEM_H
#define UNTITLED_PLAYERINPUTSYSTEM_H


#include "System.h"

class PlayerInputSystem : public System {

public:
    void update(EntityManager &entities, double dt) override;
};


#endif //UNTITLED_PLAYERINPUTSYSTEM_H
