#include <iostream>
#include "PhysicsSystem.h"
#include "../Components/Kinematics.h"
#include "../Components/Transform.h"
#include "../Components/GravityForce.h"
#include "../Components/Child.h"


void PhysicsSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<Kinematics, Transform>()) {
        for (Entity *gravityEntity: entities.getEntitiesWith<GravityForce, Transform>()) {
            Vector3 diff = gravityEntity->get<Transform>()->position - entity->get<Transform>()->position;
            double distance = diff.magnitude();

            double forceMagnitude =
                    gravityEntity->get<GravityForce>()->mass / pow(distance, 2);

            Vector3 force = diff.normalize().scale(forceMagnitude);
            entity->get<Kinematics>()->acceleration += force;
        }

        Transform *transform = entity->get<Transform>();
        Kinematics *kinematics = entity->get<Kinematics>();

        kinematics->acceleration -= kinematics->velocity * kinematics->drag;

        kinematics->velocity += kinematics->acceleration * dt / 1000;
        transform->position += kinematics->velocity * dt / 1000;

//        transform->rotation += kinematics->angularVelocity * dt / 1000;


        kinematics->acceleration = Vector3(0, 0);

        if(entity->has<Child>()) {
            Entity *child = entity->get<Child>()->child;
            if (child->has<Transform>()) {
                child->get<Transform>()->position = transform->position;
            }
        }

    }
}
