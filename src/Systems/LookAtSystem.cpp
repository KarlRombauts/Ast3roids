//
// Created by Karl Rombauts on 4/6/21.
//

#include <Components/LookAt.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include "LookAtSystem.h"

void LookAtSystem::update(EntityManager &entities) {
    for (Entity *source: entities.getEntitiesWith<LookAt, Position>()) {
        LookAt *lookAt = source->get<LookAt>();
        Entity *target = lookAt->target;

        if (target->has<Position>()) {
            Vector3 &targetPos = target->get<Position>()->position;
            Vector3 &sourcePos = source->get<Position>()->position;

            Quaternion rotation = Quaternion::lookRotation(
                    Vector3::fromTo(sourcePos, targetPos), Vector3::up());

            // Spin the billboard in its own plane (about the view axis) so
            // repeated sprites don't all look identical.
            rotation *= Quaternion::angleAxis(lookAt->roll, Vector3::forward());

            source->assign<Rotation>(rotation);
        }
    }

}
