#include "BulletCleanupSystem.h"
#include "../Components/Impact.h"
#include "../Components/Bullet.h"
#include "../Components/Collision.h"
#include "../Components/SpaceShip.h"
#include "../Components/Destroy.h"

void BulletCleanupSystem::update(EntityManager &entities, double dt) {
    for (Entity *bullet: entities.getEntitiesWith<Impact, Bullet>()) {
        std::vector<Entity *> otherEntities = bullet->get<Impact>()->entities;

        for (Entity *otherEntity: otherEntities) {
            if (otherEntity->has<SpaceShip>()) {
                continue;
            }
            if (otherEntity->get<Collision>()->type == CollisionType::TRIGGER) {
                continue;
            }
            bullet->assign<Destroy>();
            return;
        }
    }
}
