//
// Created by Karl Rombauts on 16/4/21.
//

#ifndef UNTITLED_ASTEROIDSYSTEM_H
#define UNTITLED_ASTEROIDSYSTEM_H


#include "../ecs/EntityManager.h"
#include "../GameModel.h"
#include "../Components/SpaceShip.h"
#include "../Components/Transform.h"
#include "../Components/Kinematics.h"
#include "../Helpers.h"
#include "../Components/OutsideArena.h"

class AsteroidSystem {
private:
    void launchAsteroidAtSpaceShip(EntityManager &entities, Entity *spaceShip) const;
public:
    void startWave(EntityManager &entities, int waveCount);
};


#endif //UNTITLED_ASTEROIDSYSTEM_H
