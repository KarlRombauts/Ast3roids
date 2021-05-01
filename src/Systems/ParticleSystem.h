#ifndef UNTITLED_PARTICLESYSTEM_H
#define UNTITLED_PARTICLESYSTEM_H


#include "../ecs/EntityManager.h"

class ParticleSystem {
public:
    void update(EntityManager &entities, double dt);

    void emitParticle(EntityManager &entities, Entity *emitterEntity) const;
};


#endif //UNTITLED_PARTICLESYSTEM_H
