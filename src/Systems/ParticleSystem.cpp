#include <Components/Rotation.h>
#include "ParticleSystem.h"
#include "../Components/ParticleSource.h"
#include "../Components/Position.h"
#include "../Components/Particle.h"
#include "../Components/Kinematics.h"
#include "../Helpers.h"
#include "../Components/Collision.h"
#include "../Components/Destroy.h"
#include "Components/Scale.h"

void ParticleSystem::update(EntityManager &entities, double dt) {
    for (Entity *emitter: entities.getEntitiesWith<ParticleSource, Position>()) {
        ParticleSource *particleSource = emitter->get<ParticleSource>();
        for (int i = 0; i < particleSource->initialParticles; i++) {
            emitParticle(entities, emitter);
        }
        emitter->assign<Destroy>();
    }

    for (Entity *particleEntity: entities.getEntitiesWith<Particle>()) {
        Color *texture = particleEntity->get<Color>();
        Particle *particle = particleEntity->get<Particle>();

        double decayRate = particle->decayRate * dt / 1000;

        texture->red = lerp(texture->red, particle->deathColor.red, decayRate);
        texture->green = lerp(texture->green, particle->deathColor.green, decayRate);
        texture->blue = lerp(texture->blue, particle->deathColor.blue, decayRate);
        particle->size = lerp(particle->size, particle->deathSize, decayRate);

        if (texture->red + texture->blue + texture->green < 0.1) {
            particleEntity->assign<Destroy>();
        }
    }
}

void ParticleSystem::emitParticle(EntityManager &entities, Entity *emitterEntity) const {
    ParticleSource *particleSource = emitterEntity->get<ParticleSource>();
    Entity *particleEntity = entities.create();

    Particle particle;
    particle.size = randf(0.2, 0.5);
    particle.deathSize = 0;
    particle.decayRate = particleSource->decayRate;
    particle.deathColor = {0, 0, 0};

    particleEntity->assign<Particle>(particle);

    particleEntity->assign<Position>(*emitterEntity->get<Position>());
    particleEntity->assign<Scale>(particle.size);
    particleEntity->assign<Rotation>();

    particleEntity->assign<Collision>(CollisionType::DYNAMIC);
    particleEntity->assign<CircleCollision>(3);

    double d = particleSource->dispersion;
    Vector3 velocity = Vector3::random(d + randf(-d / 3, d / 3)) + particleSource->velocity;
    particleEntity->assign<Kinematics>(velocity, Vector3(0, 0, 0), 0);
    particleEntity->assign<Color>(1, 1, 1);
}
