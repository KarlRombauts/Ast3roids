#ifndef UNTITLED_SPACESHIP_H
#define UNTITLED_SPACESHIP_H

#include <ecs/Component.h>

struct SpaceShip: public Component {
    SpaceShip(int fireRate) : lastFire(0), fireRate(fireRate) {};

    int lastFire;
    int fireRate;

    // 0 = idle, 1 = full thrust. Drives the engine-glow intensity (smoothed in
    // PlayerInputSystem).
    double thrust = 0;
};

#endif //UNTITLED_SPACESHIP_H
