#include <Components/Rotation.h>
#include <Components/PlayerInput.h>
#include "PhysicsSystem.h"
#include "../Components/Kinematics.h"
#include "../Components/Position.h"
#include "../Components/Child.h"


void PhysicsSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<Kinematics, Position>()) {
        updateVelocity(entity, dt);
        updatePosition(entity, dt);
        updateRotation(entity, dt);
        updateChild(entity);
    }
}

void PhysicsSystem::updateChild(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    if(entity->has<Child>()) {
        Entity *child = entity->get<Child>()->child;
        if (child->has<Position>()) {
            child->get<Position>()->position = position;
        }
    }
}

void PhysicsSystem::updatePosition(Entity *entity, double dt) const {
    Vector3 &velocity = entity->get<Kinematics>()->velocity;
    Vector3 &position = entity->get<Position>()->position;

    position += velocity * dt / 1000;
}

void PhysicsSystem::updateVelocity(Entity *entity, double dt) const {
    Vector3 &velocity = entity->get<Kinematics>()->velocity;
    Vector3 &acceleration = entity->get<Kinematics>()->acceleration;
    double drag = entity->get<Kinematics>()->drag;

    acceleration -= velocity * drag;
    velocity += acceleration * dt / 1000;
    acceleration = Vector3(0, 0, 0);
}

void PhysicsSystem::updateRotation(Entity *entity, double dt) const {
    Quaternion &rotation = entity->get<Rotation>()->rotation;
    Vector3 &angularVelocity = entity->get<Kinematics>()->angularVelocity;

    if (!entity->has<PlayerInput>()) {
        double angle = angularVelocity.magnitude() * dt / 1000;
        if (angle > 0) {
            rotation *= Quaternion::angleAxis(angle, angularVelocity);
        }
    }
}
