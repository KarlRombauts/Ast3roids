#ifndef STARFOX_ANIMATEDTEXTURESYSTEM_H
#define STARFOX_ANIMATEDTEXTURESYSTEM_H


#include "System.h"

class AnimatedTextureSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

    double timeSinceLastFrame = 0;

};


#endif //STARFOX_ANIMATEDTEXTURESYSTEM_H
