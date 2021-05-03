#ifndef STARFOX_SMOOTHFOLLOWSYSTEM_H
#define STARFOX_SMOOTHFOLLOWSYSTEM_H


#include "System.h"

class SmoothFollowSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

};


#endif //STARFOX_SMOOTHFOLLOWSYSTEM_H
