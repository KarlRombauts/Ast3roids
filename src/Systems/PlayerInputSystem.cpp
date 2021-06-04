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

void PlayerInputSystem::update(EntityManager &entities, double dt) {
    for(Entity* entity: entities.getEntitiesWith<Position, Rotation, Kinematics, Geometry, PlayerInput, SpaceShip>()) {
        Vector3 &position = entity->get<Position>()->position;
        Geometry *geometry = entity->get<Geometry>();
        Quaternion &rotation = entity->get<Rotation>()->rotation;
        Vector3 &acceleration = entity->get<Kinematics>()->acceleration;
        Vector3 &velocity = entity->get<Kinematics>()->velocity;

        if (keyboardState.isKeyPressed('k')) {
            Vector3 &cameraOffset = gameModel.activeCamera->get<SmoothFollow>()->relativeOffset;
            if (cameraOffset.magnitude() < 40) {
                cameraOffset += cameraOffset.normalize();
            }
        }

        if (keyboardState.isKeyPressed('i')) {
            Vector3 &cameraOffset = gameModel.activeCamera->get<SmoothFollow>()->relativeOffset;
            if (cameraOffset.magnitude() > 10) {
                cameraOffset -= cameraOffset.normalize();
            }
        }

        if (keyboardState.isKeyPressed('a')) {
            rotation *= Quaternion::angleAxis(gameConfig.PLAYER_TURN_SPEED * dt / 1000, Vector3::forward());
        }

        if (keyboardState.isKeyPressed('d')) {
            rotation *= Quaternion::angleAxis(-gameConfig.PLAYER_TURN_SPEED * dt / 1000, Vector3::forward());
        }

        if (keyboardState.isKeyPressed('w')) {
            Vector3 force = Vector3::back() * gameConfig.PLAYER_SPEED * dt / 1000;
            acceleration += rotation * force;

            openWings(geometry);
        }

        if (!keyboardState.isKeyPressed('w')) {
            closeWings(geometry);
        }

//        if (keyboardState.isKeyPressed(gameConfig.PLAYER_FORWARD)) {
//            kinematics->acceleration = Vector3::polar(Position->rotation, spaceShip->thrust);

            // Create exhaust particle system
//            Entity* particles = entities.create();
//
//            Vector3 initialVelocity = kinematics->acceleration.scale(-1) * dt / 1000;
//            particles->assign<ParticleSource>(
//                    initialVelocity,
//                    5,
//                    gameConfig.EXHAUST_PARTICLE_RATE,
//                    gameConfig.EXHAUST_PARTICLE_DECAY);
//
//            // Offset particles
//            Position particlesPosition = *position;
//            particlesPosition.position += kinematics->acceleration.normalize().scale(-3);
//            particles->assign<Position>(particlesPosition);
//        }

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
