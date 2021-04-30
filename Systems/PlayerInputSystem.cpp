#include "PlayerInputSystem.h"
#include "../Globals.h"
#include "../Components/PlayerInput.h"
#include "../Components/Transform.h"
#include "../Components/Kinematics.h"
#include "../Components/SpaceShip.h"
#include "../Components/FiringBullet.h"
#include "../Components/ParticleSource.h"

void PlayerInputSystem::update(EntityManager &entities, double dt) {
    for(Entity* entity: entities.getEntitiesWith<Transform, Kinematics, PlayerInput, SpaceShip>()) {
        Transform *transform = entity->get<Transform>();
        Kinematics *kinematics = entity->get<Kinematics>();
        SpaceShip *spaceShip = entity->get<SpaceShip>();

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_LEFT)) {
            transform->rotation += (float) gameConfig.PLAYER_TURN_SPEED * dt / 1000;
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_RIGHT)) {
            transform->rotation -= (float) gameConfig.PLAYER_TURN_SPEED * dt / 1000;
        }

        if (keyboardState.isKeyPressed(gameConfig.PLAYER_FORWARD)) {
            kinematics->acceleration = Vec3::polar(transform->rotation, spaceShip->thrust);

            // Create exhaust particle system
            Entity* particles = entities.create();

            Vec3 initialVelocity = kinematics->acceleration.scale(-1) * dt / 1000;
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
