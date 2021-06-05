#include <Factory/ImpactFactory.h>
#include <Factory/ExplosionFactory.h>
#include "DamageSystem.h"
#include "../Components/Impact.h"
#include "../Components/Damage.h"
#include "../Components/Health.h"
#include "../Components/SplitOnDeath.h"
#include "../Components/Asteroid.h"
#include "../Components/Position.h"
#include "../Components/Kinematics.h"
#include "../Components/ParticleSource.h"
#include "../GameModel.h"
#include "../Components/Destroy.h"
#include "../Globals.h"
#include "Factory/AsteroidFactory.h"

void DamageSystem::update(EntityManager &entities) {
    for(Entity* entity: entities.getEntitiesWith<Impact, Health>()) {
        int &health = entity->get<Health>()->health;

        // Loop over impacts
        for (Entity* otherEntity: entity->get<Impact>()->entities) {
            if (otherEntity->has<Damage>()) {
               health -= otherEntity->get<Damage>()->damage;
            }

            if (health <= 0) {
                handleDeath(entities, entity, otherEntity);
                break;
            }
        }
    }
}

void DamageSystem::handleDeath(EntityManager &entities, Entity *entity, Entity *otherEntity) {
    if (entity->has<Asteroid, Position>()) {
        if (entity->has<SplitOnDeath>()) {
            splitAsteroid(entities, entity, calculateSplitDirection(otherEntity));
        }

        createExplosion(entities, entity);
        gameModel.score++;
    }

    entity->assign<Destroy>();
}

Vector3 DamageSystem::calculateSplitDirection(Entity *impactObject) const {
    Vector3 randVector = Vector3::random(1);
    Vector3 bulletVelocity = impactObject->get<Kinematics>()->velocity;
    Vector3 splitDir = bulletVelocity.cross(randVector).cross(bulletVelocity).normalize();
    return splitDir;
}

void DamageSystem::createExplosion(EntityManager &entities, Entity *asteroid) {
    double size = asteroid->get<Asteroid>()->size;
    Vector3 p1 = asteroid->get<Position>()->position;

    // Explosion Texture
    ExplosionFactory::create(entities, p1, size * 2.5);

    // Particles
    Entity* particles = entities.create();
    particles->assign<ParticleSource>(Vector3(0, 0, 0), 20, size * 5, gameConfig.EXPLOSION_DECAY_RATE);
    particles->assign<Position>(p1);
}

void DamageSystem::splitAsteroid(EntityManager &entities, Entity *asteroid, const Vector3 &splitDir) {
    double size = asteroid->get<Asteroid>()->size;
    Vector3 p1 = asteroid->get<Position>()->position;
    Vector3 v = asteroid->get<Kinematics>()->velocity;

    Entity *asteroid1 = AsteroidFactory::create(entities, size / 1.5);
    Entity *asteroid2 = AsteroidFactory::create(entities, size / 1.5);

    asteroid1->get<Position>()->position = p1 + splitDir.scale(size / 1.25);
    asteroid1->get<Kinematics>()->velocity = (v + splitDir.scale(v.magnitude())).normalize().scale(v.magnitude());

    asteroid2->get<Position>()->position = p1 - splitDir.scale(size / 1.25);
    asteroid2->get<Kinematics>()->velocity = (v - splitDir.scale(v.magnitude())).normalize().scale(v.magnitude());
}
