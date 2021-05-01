#ifndef UNTITLED_PARTICLE_H
#define UNTITLED_PARTICLE_H


#include "Texture.h"

struct Particle: public Component {
    double size = 3;
    double deathSize = 3;
    double decayRate = 0;
    Texture deathColor = {1, 1, 1};
};


#endif //UNTITLED_PARTICLE_H
