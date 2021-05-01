#include "DestroySystem.h"

void DestroySystem::update(EntityManager &entities) {
    for(Entity *entity: entities.getEntitiesWith<Destroy>()) {
        entities.destroy(entity);
    }
}
