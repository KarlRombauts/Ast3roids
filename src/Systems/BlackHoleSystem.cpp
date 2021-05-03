#include "BlackHoleSystem.h"
#include "../Components/BlackHole.h"
#include "../Components/Transform.h"
#include "../GameModel.h"
#include "../Components/Impact.h"
#include "../Components/Collision.h"
#include "../Components/Destroy.h"
#include "../Components/BoundingCircle.h"
#include "../Globals.h"

void BlackHoleSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<BlackHole, Transform, CircleCollision>()) {
        Transform *transform = entity->get<Transform>();

        double pulseTime = 1 / gameConfig.BLACK_HOLE_PULSE_RATE;
        double minSize = 0.7;
        double scale = (1 - ((fmod(gameModel.elapsedTime / (double) 1000, pulseTime)) / pulseTime) + minSize) / (1 + minSize);

        transform->scale = Vector3(scale, scale, scale);
    }

    for (Entity *blackHole: entities.getEntitiesWith<BlackHole, Impact>()) {
        for (Entity *otherEntity: blackHole->get<Impact>()->entities) {
            // We do not want the bounding circle to be destroyed
            if (!otherEntity->has<BoundingCircle>()) {
                otherEntity->assign<Destroy>();
            }
        }
    }
}
