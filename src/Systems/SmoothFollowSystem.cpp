#include <Components/SmoothFollow.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Position.h>
#include "SmoothFollowSystem.h"

void SmoothFollowSystem::update(EntityManager &entities, double dt) {
    for (Entity * follower: entities.getEntitiesWith<SmoothFollow, Position, Rotation>()) {
        Entity * target = follower->get<SmoothFollow>()->entity;
        Vector3 &relativeOffset = follower->get<SmoothFollow>()->relativeOffset;


        if (!target->has<Position, Rotation>()) {
            // Target cannot be followed
            continue;
        }

        Vector3 &currentPosition = follower->get<Position>()->position;
        Quaternion &currentRotation = follower->get<Rotation>()->rotation;

        Quaternion &targetRotation = target->get<Rotation>()->rotation;
        Vector3 targetPosition = target->get<Position>()->position + targetRotation * relativeOffset;

        currentPosition = Vector3::lerp(currentPosition, targetPosition, 0.1);
        currentRotation = Quaternion::slerp(currentRotation, targetRotation, 0.1);
    }

}
