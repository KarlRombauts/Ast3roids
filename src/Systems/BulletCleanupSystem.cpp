#include "BulletCleanupSystem.h"
#include "../Components/Impact.h"
#include "../Components/Bullet.h"
#include "../Components/Collision.h"
#include "../Components/SpaceShip.h"
#include "../Components/Destroy.h"

void BulletCleanupSystem::update(EntityManager &entities, double dt) {
    for (Entity *bullet: entities.getEntitiesWith<Impact, Bullet>()) {
        for (Entity *impactEntity: bullet->get<Impact>()->entities) {
            if (impactEntity->has<SpaceShip>()) continue;
            if (impactEntity->get<Collision>()->type == CollisionType::TRIGGER) continue;

            bullet->assign<Destroy>();
            return;
        }
    }
}
