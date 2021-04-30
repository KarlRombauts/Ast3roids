#include "ParticleSystem.h"
#include "../Components/ParticleSource.h"
#include "../Components/Transform.h"
#include "../Components/Particle.h"
#include "../Components/Kinematics.h"
#include "../Helpers.h"
#include "../Components/Collision.h"
#include "../Components/Destroy.h"

void ParticleSystem::update(EntityManager &entities, double dt) {
    for (Entity *emitter: entities.getEntitiesWith<ParticleSource, Transform>()) {
        ParticleSource *particleSource = emitter->get<ParticleSource>();
        for (int i = 0; i < particleSource->initialParticles; i++) {
            emitParticle(entities, emitter);
        }
        emitter->assign<Destroy>();
    }

    for (Entity *particleEntity: entities.getEntitiesWith<Particle>()) {
        Texture *texture = particleEntity->get<Texture>();
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
    particle.size = 3;
    particle.deathSize = 3;
    particle.decayRate = particleSource->decayRate;
    particle.deathColor = {0, 0, 0};

    particleEntity->assign<Particle>(particle);
    particleEntity->assign<Transform>(*emitterEntity->get<Transform>());
    particleEntity->assign<Collision>(CollisionType::DYNAMIC);
    particleEntity->assign<CircleCollision>(3);

    double d = particleSource->dispersion;
    Vec3 velocity = Vec3::polar(randf(0, 360), d + randf(-d / 3, d / 3)) +
                    particleSource->velocity;
    particleEntity->assign<Kinematics>(velocity, Vec3(0, 0), 0);
    particleEntity->assign<Texture>(1, 1, 1);
}
