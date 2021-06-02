#include <iostream>
#include <Components/Rotation.h>
#include <Components/PlayerInput.h>
#include "PhysicsSystem.h"
#include "../Components/Kinematics.h"
#include "../Components/Position.h"
#include "../Components/GravityForce.h"
#include "../Components/Child.h"


void PhysicsSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<Kinematics, Position>()) {
//        for (Entity *gravityEntity: entities.getEntitiesWith<GravityForce, Position>()) {
//            Vector3 diff = gravityEntity->get<Position>()->position - entity->get<Position>()->position;
//            double distance = diff.magnitude();
//
//            double forceMagnitude = gravityEntity->get<GravityForce>()->mass / pow(distance, 2);
//
//            Vector3 force = diff.normalize().scale(forceMagnitude);
//            entity->get<Kinematics>()->acceleration += force;
//        }

        Vector3 &position = entity->get<Position>()->position;
        Kinematics *kinematics = entity->get<Kinematics>();
        Quaternion &rotation = entity->get<Rotation>()->rotation;
        Vector3 &angularVelocity = entity->get<Kinematics>()->angularVelocity;

        kinematics->acceleration -= kinematics->velocity * kinematics->drag;

        // TODO: Fix velocity
        kinematics->velocity += kinematics->acceleration * dt / 1000;
        position += kinematics->velocity * dt / 1000;

//        if (!entity->has<PlayerInput>()) {
//            double angle = angularVelocity.magnitude() * dt / 1000;
//            rotation *= Quaternion::angleAxis(angle, angularVelocity);
//        }

        kinematics->acceleration = Vector3(0, 0, 0);

        if(entity->has<Child>()) {
            Entity *child = entity->get<Child>()->child;
            if (child->has<Position>()) {
                child->get<Position>()->position = position;
            }
        }
    }
}
