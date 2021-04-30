#include "OutOfBoundsSystem.h"
#include "../Components/Transform.h"
#include "../GameModel.h"
#include "../Components/Destroy.h"
#include "../Components/Asteroid.h"

void OutOfBoundsSystem::update(EntityManager &entities) {
    for (Entity *asteroid: entities.getEntitiesWith<Transform, Asteroid>()) {
        Vec3 position = asteroid->get<Transform>()->position;
        if (position.magnitude() > gameModel.worldCoordinates.distanceToCorner() + 30) {
            asteroid->assign<Destroy>();
        }
    }
}
