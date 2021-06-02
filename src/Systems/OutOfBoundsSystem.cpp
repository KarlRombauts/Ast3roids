#include "OutOfBoundsSystem.h"
#include "../Components/Position.h"
#include "../GameModel.h"
#include "../Components/Destroy.h"
#include "../Components/Asteroid.h"

void OutOfBoundsSystem::update(EntityManager &entities) {
    for (Entity *asteroid: entities.getEntitiesWith<Position, Asteroid>()) {
        Vector3 position = asteroid->get<Position>()->position;
        if (position.magnitude() > gameModel.worldCoordinates.distanceToCorner() + 30) {
            asteroid->assign<Destroy>();
        }
    }
}
