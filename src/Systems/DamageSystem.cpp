#include "DamageSystem.h"
#include "../Components/Impact.h"
#include "../Components/Damage.h"
#include "../Components/Health.h"
#include "../Components/SplitOnDeath.h"
#include "../Components/Asteroid.h"
#include "../Components/Transform.h"
#include "../Components/Kinematics.h"
#include "../Components/ParticleSource.h"
#include "../GameModel.h"
#include "../Components/Destroy.h"
#include "../Globals.h"

void DamageSystem::update(EntityManager &entities) {
    for(Entity* entity: entities.getEntitiesWith<Impact, Health>()) {
        Health *health = entity->get<Health>();

        for (Entity* otherEntity: entity->get<Impact>()->entities) {
            if (otherEntity->has<Damage>()) {
               health->health -= otherEntity->get<Damage>()->damage;
            }

            if (health->health <= 0) {
                handleDeath(entities, entity, otherEntity);
                break;
            }
        }
    }
}

void DamageSystem::handleDeath(EntityManager &entities, Entity *entity, Entity *otherEntity) const {
    if (entity->has<SplitOnDeath>()) {
        if (entity->has<Asteroid, Transform>()) {
            double size = entity->get<Asteroid>()->size;
            Vector3 p1 = entity->get<Transform>()->position;
            Vector3 p2 = otherEntity->get<Transform>()->position;
            Vector3 v = entity->get<Kinematics>()->velocity;

            Vector3 splitDir = (p2 - p1).perpendicular() * size / 2;

            if (size >= 2) {
                Entity *asteroid1 = entities.createAsteroid(size / 2);
                Entity *asteroid2 = entities.createAsteroid(size / 2);

                asteroid1->get<Transform>()->position = p1 + splitDir;
                asteroid1->get<Kinematics>()->velocity = (p2 - p1).normalize().scale(v.magnitude()).rotate(25);
                asteroid2->get<Transform>()->position = p1 - splitDir;
                asteroid1->get<Kinematics>()->velocity = (p2 - p1).normalize().scale(v.magnitude()).rotate(-25);
            }

        }
    }

    if (entity->has<Asteroid>()) {
        // Create particle emitter
        double size = entity->get<Asteroid>()->size;
        Entity* particles = entities.create();
        particles->assign<ParticleSource>(Vector3(0, 0), 20, size * 5, gameConfig.EXPLOSION_DECAY_RATE);
        particles->assign<Transform>(*entity->get<Transform>());

        // Increment Score
        gameModel.score++;
    }

    entity->assign<Destroy>();
}
