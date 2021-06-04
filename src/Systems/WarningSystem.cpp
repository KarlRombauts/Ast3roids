#include <iostream>
#include <Components/Material.h>
#include "WarningSystem.h"
#include "../Components/Impact.h"
#include "../Components/SpaceShip.h"
#include "../Components/BoundingCircle.h"
#include "../Components/Wall.h"
#include "../Components/Color.h"

void WarningSystem::update(EntityManager &entities) {
    for(Entity *wall: entities.getEntitiesWith<Wall>()) {
        Material* material = wall->get<Material>();
        material->setEmission(0.2, 0.2, 0.2);
    }

    for (Entity *entity: entities.getEntitiesWith<Impact, BoundingCircle>()) {
        std::vector<Entity *> otherEntities = entity->get<Impact>()->entities;

        for (Entity *otherEntity: otherEntities) {
            if (otherEntity->has<Wall>()) {
                Material* material = otherEntity->get<Material>();
                material->setEmission(1, 0, 0);
            }
        }
    }
}
