#include <iostream>
#include "WarningSystem.h"
#include "../Components/Impact.h"
#include "../Components/SpaceShip.h"
#include "../Components/BoundingCircle.h"
#include "../Components/Wall.h"
#include "../Components/Texture.h"

void WarningSystem::update(EntityManager &entities) {
    for(Entity *wall: entities.getEntitiesWith<Wall>()) {
        wall->assign<Texture>(1, 1, 1);
    }

    for (Entity *entity: entities.getEntitiesWith<Impact, BoundingCircle>()) {
        std::vector<Entity *> otherEntities = entity->get<Impact>()->entities;

        for (Entity *otherEntity: otherEntities) {
            if (otherEntity->has<Wall>()) {
                otherEntity->assign<Texture>(1, 0, 0);
            }
        }
    }
}
