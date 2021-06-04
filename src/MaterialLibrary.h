#ifndef STARFOX_MATERIALLIBRARY_H
#define STARFOX_MATERIALLIBRARY_H


#include <Components/Geometry.h>
#include <Helpers/TextureLoader.h>
#include "Components/Material.h"

class MaterialLibrary {
public:
    Material *ASTEROID;
    Material *GLOW_PARTICLE;
    Material *BULLET;
    Material *EXPLOSION;

    void init();
};

#endif //STARFOX_MATERIALLIBRARY_H
