//
// Created by Karl Rombauts on 4/6/21.
//

#include <Components/LookAt.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include "LookAtSystem.h"

void LookAtSystem::update(EntityManager &entities) {
    for (Entity *source: entities.getEntitiesWith<LookAt, Position>()) {
        Entity *target = source->get<LookAt>()->target;
        
        if (target->has<Position>()) {
            Vector3 &targetPos = target->get<Position>()->position;
            Vector3 &sourcePos = source->get<Position>()->position;

            Quaternion rotation = Quaternion::lookRotation(
                    Vector3::fromTo(sourcePos, targetPos), Vector3::up());

            source->assign<Rotation>(rotation);
        }
    }

}
