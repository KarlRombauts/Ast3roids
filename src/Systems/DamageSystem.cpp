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
    if (entity->has<Asteroid, Position>()) {
        double size = entity->get<Asteroid>()->size;
        Vector3 p1 = entity->get<Position>()->position;
        Vector3 v = entity->get<Kinematics>()->velocity;
        entities.createExplosion(p1, size * 5);

        if (entity->has<SplitOnDeath>()) {
            Vector3 randVector = Vector3::random(1);
            Vector3 bulletVelocity = otherEntity->get<Kinematics>()->velocity;
            Vector3 splitDir = bulletVelocity.cross(randVector).cross(bulletVelocity).normalize();

            if (size >= 2) {
                Entity *asteroid1 = entities.createAsteroid(size / 1.5);
                Entity *asteroid2 = entities.createAsteroid(size / 1.5);

                asteroid1->get<Position>()->position = p1 + splitDir.scale(size / 1.25);
                asteroid1->get<Kinematics>()->velocity = (v + splitDir.scale(v.magnitude())).normalize().scale(v.magnitude());;

                asteroid2->get<Position>()->position = p1 - splitDir.scale(size / 1.25);
                asteroid2->get<Kinematics>()->velocity = (v - splitDir.scale(v.magnitude())).normalize().scale(v.magnitude());
            }
        }
    }

    if (entity->has<Asteroid>()) {
        // Create particle emitter
        double size = entity->get<Asteroid>()->size;
        Entity* particles = entities.create();
        particles->assign<ParticleSource>(Vector3(0, 0, 0), 20, size * 5, gameConfig.EXPLOSION_DECAY_RATE);
        particles->assign<Position>(*entity->get<Position>());
        // Increment Score
        gameModel.score++;
    }

    entity->assign<Destroy>();
}
