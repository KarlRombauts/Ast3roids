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
#include "RayCastingSystem.h"

void RayCastingSystem::update(EntityManager &entities) {
    for (Entity *entity: entities.getEntitiesWith<Camera, Position, Rotation>()) {
        Camera *camera = entity->get<Camera>();
        Vector3 &position = entity->get<Position>()->position;
        Quaternion &rotation = entity->get<Rotation>()->rotation;
        Vector2 mouse = mouseState.position;


        Vector2 nearClipPlane;
        nearClipPlane.y = camera->nearClip * tan(camera->fov);
        nearClipPlane.x = camera->aspectRatio * nearClipPlane.y;

        Vector3 mouseOnClipPlane(0, 0, camera->nearClip);

        mouseOnClipPlane.x = map(mouse.x, {0, gameModel.width}, {-1, 1});

        mouseOnClipPlane.y = map(mouse.y, {0, gameModel.height}, {-1, 1});

        // Todo: Add a tilt speed based on dt
        rotation *= Quaternion::angleAxis(4 * mouseOnClipPlane.y, Vector3::left());
        rotation *= Quaternion::angleAxis(4 * mouseOnClipPlane.x, Vector3::up());
    }
}
