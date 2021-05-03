#include <Components/Camera.h>
#include <Components/Position.h>
#include "PlayerInputSystem.h"
#include "../Globals.h"
#include "../Components/PlayerInput.h"
#include "../Components/Transform.h"
#include "../Components/Kinematics.h"
#include "../Components/SpaceShip.h"
#include "../Components/FiringBullet.h"
#include "../Components/ParticleSource.h"
#include "../Components/Rotation.h"

void PlayerInputSystem::update(EntityManager &entities, double dt) {
    for(Entity* entity: entities.getEntitiesWith<Camera, Rotation, Position>()) {
        Vector3 &position = entity->get<Position>()->position;
        Quaternion &rotation = entity->get<Rotation>()->rotation;


        if (keyboardState.isKeyPressed('a')) {
            Vector3 localMove = Vector3::right() * 10 * dt / 1000;
            position += rotation.conjugate() * localMove;
        }

        if (keyboardState.isKeyPressed('d')) {
            Vector3 localMove = Vector3::left() * 10 * dt / 1000;
            position += rotation.conjugate() * localMove;
        }

        if (keyboardState.isKeyPressed('w')) {
            Vector3 localMove = Vector3::back() * 10 * dt / 1000;
            position += rotation.conjugate() * localMove;
        }
        if (keyboardState.isKeyPressed('s')) {
            Vector3 localMove = Vector3::forward() * 10 * dt / 1000;
            position += rotation.conjugate() * localMove;
        }
    }

    for(Entity* entity: entities.getEntitiesWith<Transform, Rotation, Kinematics, PlayerInput, SpaceShip>()) {
        Transform *transform = entity->get<Transform>();
        Kinematics *kinematics = entity->get<Kinematics>();
        SpaceShip *spaceShip = entity->get<SpaceShip>();
        Quaternion &rotation = entity->get<Rotation>()->rotation;

//        rotation = Quaternion::lookRotation(transform->position * -1, Vector3::forward());
//        rotation = Quaternion::slerp(rotation, Quaternion::angleAxis(180, Vector3::forward()), 0.001);
        double angle = (float) gameConfig.PLAYER_TURN_SPEED * dt / 1000;


        if (keyboardState.isKeyPressed('k')) {
//            rotation *= Quaternion::angleAxis(-angle, Vector3::up());
            transform->position += Vector3::back() * 10 * dt / 1000;
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_FORWARD)) {
            kinematics->acceleration = Vector3::polar(transform->rotation, spaceShip->thrust);

            // Create exhaust particle system
            Entity* particles = entities.create();

            Vector3 initialVelocity = kinematics->acceleration.scale(-1) * dt / 1000;
            particles->assign<ParticleSource>(
                    initialVelocity,
                    5,
                    gameConfig.EXHAUST_PARTICLE_RATE,
                    gameConfig.EXHAUST_PARTICLE_DECAY);

            // Offset particles
            Transform particlesTransform = *transform;
            particlesTransform.position += kinematics->acceleration.normalize().scale(-3);
            particles->assign<Transform>(particlesTransform);
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_SHOOT)) {
            entity->assign<FiringBullet>();
        }
    }
}
