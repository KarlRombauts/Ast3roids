#ifndef UNTITLED_SPACESHIP_H
#define UNTITLED_SPACESHIP_H

#include <ecs/Component.h>

struct SpaceShip: public Component {
    SpaceShip(int fireRate) : lastFire(0), fireRate(fireRate) {};

    int lastFire;
    int fireRate;
};

#endif //UNTITLED_SPACESHIP_H
