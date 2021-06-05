#include <Components/Rotation.h>
#include <Components/Geometry.h>
#include <GameModel.h>
#include <Components/LookAt.h>
#include <Components/Transparency.h>
#include <Factory/Primatives/PlaneFactory.h>
#include <Globals.h>
#include "ParticleSystem.h"
#include "../Components/ParticleSource.h"
#include "../Components/Position.h"
#include "../Components/Particle.h"
#include "../Components/Kinematics.h"
#include "../Helpers.h"
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
        Particle *particle = particleEntity->get<Particle>();

        double decayRate = particle->decayRate * dt / 1000;

        particle->size = lerp(particle->size, particle->deathSize, decayRate);

        particleEntity->assign<Scale>(particle->size);

        if (particle->size < 0.1) {
            particleEntity->assign<Destroy>();
        }
    }
}

void ParticleSystem::emitParticle(EntityManager &entities, Entity *emitterEntity) const {
    ParticleSource *particleSource = emitterEntity->get<ParticleSource>();
    Entity *particleEntity = entities.create();

    Particle particle;
    particle.size = randf(1, 2);
    particle.deathSize = 0;
    particle.decayRate = particleSource->decayRate;

    particleEntity->assign<Particle>(particle);

    particleEntity->assign<Position>(*emitterEntity->get<Position>());
    particleEntity->assign<Scale>(particle.size);
    particleEntity->assign<Rotation>();

    Geometry geometry = PlaneFactory::create(materialLibrary.GLOW_PARTICLE);
    particleEntity->assign<Geometry>(geometry);

    particleEntity->assign<LookAt>(gameModel.activeCamera);
    particleEntity->assign<Transparency>();

    double d = particleSource->dispersion;
    Vector3 velocity = Vector3::random(d + randf(-d / 3, d / 3)) + particleSource->velocity;
    particleEntity->assign<Kinematics>(velocity, Vector3(0, 0, 0), 0);
    particleEntity->assign<Color>(1, 1, 1);
}
