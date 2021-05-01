#ifndef UNTITLED_SPACESHIP_H
#define UNTITLED_SPACESHIP_H


struct SpaceShip: public Component {
    SpaceShip(int fireRate, int thrust): lastFire(0), fireRate(fireRate), thrust(thrust) {};

    int lastFire;
    int fireRate; // milliseconds
    double thrust;
};

#endif //UNTITLED_SPACESHIP_H
