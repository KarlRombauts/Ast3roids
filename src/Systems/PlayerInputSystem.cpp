#include <Components/Camera.h>
#include <Components/Position.h>
#include <GameModel.h>
#include <Components/SmoothFollow.h>
#include "PlayerInputSystem.h"
#include "../Globals.h"
#include "../Components/PlayerInput.h"
#include "../Components/Position.h"
#include "../Components/Kinematics.h"
#include "../Components/SpaceShip.h"
#include "../Components/FiringBullet.h"
#include "../Components/ParticleSource.h"
#include "../Components/Rotation.h"
#include "../Helpers.h"

void PlayerInputSystem::update(EntityManager &entities, double dt) {
    for(Entity* entity: entities.getEntitiesWith<Position, Rotation, Kinematics, Geometry, PlayerInput, SpaceShip>()) {
        Vector3 &position = entity->get<Position>()->position;
        Geometry *geometry = entity->get<Geometry>();
        Quaternion &rotation = entity->get<Rotation>()->rotation;
        Vector3 &acceleration = entity->get<Kinematics>()->acceleration;
        Vector3 &velocity = entity->get<Kinematics>()->velocity;

        if (keyboardState.isKeyPressed('k')) {
            Vector3 &cameraOffset = gameModel.activeCamera->get<SmoothFollow>()->relativeOffset;
            if (cameraOffset.magnitude() < gameConfig.MAX_CAMERA_DISTANCE) {
                cameraOffset += cameraOffset.normalize() * 50 *  dt / 1000;
            }
        }

        if (keyboardState.isKeyPressed('i')) {
            Vector3 &cameraOffset = gameModel.activeCamera->get<SmoothFollow>()->relativeOffset;
            if (cameraOffset.magnitude() > gameConfig.MIN_CAMERA_DISTANCE) {
                cameraOffset -= cameraOffset.normalize() * 50 * dt / 1000;
            }
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_LEFT)) {
            rotation *= Quaternion::angleAxis(gameConfig.PLAYER_TURN_SPEED * dt / 1000, Vector3::forward());
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_RIGHT)) {
            rotation *= Quaternion::angleAxis(-gameConfig.PLAYER_TURN_SPEED * dt / 1000, Vector3::forward());
        }

        // Mobile: proportional roll from banking the phone.
        if (mouseState.roll != 0.0) {
            rotation *= Quaternion::angleAxis(mouseState.roll * gameConfig.PLAYER_TURN_SPEED * dt / 1000, Vector3::forward());
        }

        double &thrust = entity->get<SpaceShip>()->thrust;
        if (keyboardState.isKeyPressed(gameConfig.PLAYER_FORWARD)) {
            Vector3 force = Vector3::back() * gameConfig.PLAYER_SPEED * dt / 1000;
            acceleration += rotation * force;

            openWings(geometry);
            thrust = lerp(thrust, 1.0, 0.1); // ramp engines up
        } else {
            closeWings(geometry);
            thrust = lerp(thrust, 0.0, 0.1); // ease engines down
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_SHOOT)) {
            entity->assign<FiringBullet>();
        }
    }
}

void PlayerInputSystem::closeWings(Geometry *geometry) {
    geometry->shapes[0].rotation = Quaternion::slerp(
            geometry->shapes[0].rotation,
            Quaternion::angleAxis(0, Vector3::forward()), 0.1);
    geometry->shapes[2].rotation = Quaternion::slerp(
            geometry->shapes[2].rotation,
            Quaternion::angleAxis(0, Vector3::forward()), 0.1);
}

void PlayerInputSystem::openWings(Geometry *geometry) {
    geometry->shapes[0].rotation = Quaternion::slerp(
            geometry->shapes[0].rotation,
            Quaternion::angleAxis(20, Vector3::forward()), 0.1);
    geometry->shapes[2].rotation = Quaternion::slerp(
            geometry->shapes[2].rotation,
            Quaternion::angleAxis(-20, Vector3::forward()), 0.1);
}
