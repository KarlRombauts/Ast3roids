#ifndef UNTITLED_PHYSICSSYSTEM_H
#define UNTITLED_PHYSICSSYSTEM_H


#include "System.h"

class PhysicsSystem: public System {

public:
    void update(EntityManager &entities, double dt) override;

    void updateRotation(Entity *entity, double dt) const;

    void updateVelocity(Entity *entity, double dt) const;

    void updatePosition(Entity *entity, double dt) const;

    void updateChild(Entity *entity) const;
};


#endif //UNTITLED_PHYSICSSYSTEM_H
