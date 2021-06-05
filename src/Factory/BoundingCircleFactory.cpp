#include <Components/BoundingCircle.h>
#include <Components/Collision.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Scale.h>
#include "BoundingCircleFactory.h"

Entity *BoundingCircleFactory::create(EntityManager &entities, double radius) {
    Entity *boundingCircle = entities.create();

    boundingCircle->assign<BoundingCircle>();

    boundingCircle->assign<Collision>(CollisionType::TRIGGER);
    boundingCircle->assign<CircleCollision>(radius);

    boundingCircle->assign<Position>();
    boundingCircle->assign<Rotation>();
    boundingCircle->assign<Scale>();

    return boundingCircle;
}

