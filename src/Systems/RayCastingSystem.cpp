//
// Created by Karl Rombauts on 3/5/21.
//

#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Globals.h>
#include <Vector2.h>
#include <Helpers.h>
#include <GameModel.h>
#include <Components/SmoothFollow.h>
#include "RayCastingSystem.h"

void RayCastingSystem::update(EntityManager &entities) {
    for (Entity *entity: entities.getEntitiesWith<Camera, Position, Rotation>()) {
        Vector3 &position = entity->get<Position>()->position;
        Quaternion &rotation = entity->get<Rotation>()->rotation;

        Vector2 mouseNorm;
        mouseNorm.x = map(mouseState.position.x, {0, gameModel.width}, {-1, 1});
        mouseNorm.y = map(mouseState.position.y, {0, gameModel.height}, {-1, 1});

        // Todo: Add a tilt speed based on dt
        if (entity->has<SmoothFollow>()) {
            Entity *target = entity->get<SmoothFollow>()->entity;

            if (target->has<Rotation>()) {
                Quaternion &targetRotation = target->get<Rotation>()->rotation;
                targetRotation = Quaternion::angleAxis(-4 * mouseNorm.y, Vector3::left()) * targetRotation;
                targetRotation = Quaternion::angleAxis(-4 * mouseNorm.x, Vector3::up()) * targetRotation;
            }
        } else {
//            rotation *= Quaternion::angleAxis(4 * mouseNorm.y, Vector3::left());
//            rotation *= Quaternion::angleAxis(4 * mouseNorm.x, Vector3::up());
        }
    }
}
