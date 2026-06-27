#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Globals.h>
#include <Vector2.h>
#include <Helpers.h>
#include <GameModel.h>
#include <Components/SmoothFollow.h>
#include "MouseLookSystem.h"

void MouseLookSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<Camera, SmoothFollow, Position, Rotation>()) {
        Vector3 &position = entity->get<Position>()->position;
        Quaternion &rotation = entity->get<Rotation>()->rotation;

        Vector2 mouseNorm;
#ifdef __EMSCRIPTEN__
        // Emscripten's SDL2 mouse coordinates are unreliable on a non-square /
        // HiDPI canvas (the Y axis is scaled against canvas width, not height),
        // which leaves a constant offset that makes the ship circle. On web we
        // instead take a normalized [-1,1] aim computed in JS from the canvas
        // rect (see web_set_aim / shell.html), which is correct at any aspect/DPR.
        mouseNorm = mouseState.aim;
#else
        mouseNorm.x = map(mouseState.position.x, {0, gameModel.width}, {-1, 1});
        mouseNorm.y = map(mouseState.position.y, {0, gameModel.height}, {-1, 1});
#endif

        Entity *target = entity->get<SmoothFollow>()->entity;

        if (target->has<Rotation>()) {
            Quaternion &targetRotation = target->get<Rotation>()->rotation;

            double t = -gameConfig.MOUSE_SENSITIVITY * dt / 1000;
            targetRotation *= Quaternion::angleAxis(t * mouseNorm.y, Vector3::left());
            targetRotation *= Quaternion::angleAxis(t * mouseNorm.x, Vector3::up());
        }
    }
}
