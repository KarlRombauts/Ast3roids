//
// Created by Karl Rombauts on 4/6/21.
//

#ifndef STARFOX_ANIMATIONSYSTEM_H
#define STARFOX_ANIMATIONSYSTEM_H


#include "System.h"

class AnimationSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;
    int timeSinceLastFrame;

    void animateLights(EntityManager &entities, int elapsedFrames);
};


#endif //STARFOX_ANIMATIONSYSTEM_H
