//
// Created by Karl Rombauts on 5/6/21.
//

#include <Components/SpaceShip.h>
#include <Globals.h>
#include <Components/Collision.h>
#include <Helpers/ObjParser.h>
#include <Components/Position.h>
#include <Components/Scale.h>
#include <Components/Rotation.h>
#include <Components/Kinematics.h>
#include <Components/PlayerInput.h>
#include <Components/Child.h>
#include "SpaceShipFactory.h"
#include "BoundingCircleFactory.h"

Entity *SpaceShipFactory::create(EntityManager &entities, const Position& position) {
    Entity *spaceShip = entities.create();

    spaceShip->assign<SpaceShip>(gameConfig.PLAYER_FIRING_RATE);
    spaceShip->assign<PlayerInput>();

    setCollisions(spaceShip);
    setGeometry(spaceShip);
    setKinematics(spaceShip);
    setTransformations(spaceShip, position);
    setBoundingCircle(entities, spaceShip);

    return spaceShip;
}

void SpaceShipFactory::setKinematics(Entity *spaceShip) {
    spaceShip->assign<Kinematics>(Vector3(0, 0, 0), Vector3(0, 0, 0), 1);
    spaceShip->get<Kinematics>()->drag = 1;
}

void SpaceShipFactory::setTransformations(Entity *spaceShip, const Position &position) {
    spaceShip->assign<Position>(position);
    spaceShip->assign<Scale>(0.5);
    spaceShip->assign<Rotation>();
}

void SpaceShipFactory::setGeometry(Entity *spaceShip) {
    Geometry geometry = ObjParser().parse(gameConfig.MODEL_DIR + "/x-wing/X-Wing-2.obj");
    spaceShip->assign<Geometry>(geometry);
}

void SpaceShipFactory::setCollisions(Entity *spaceShip) {
    spaceShip->assign<Collision>(CollisionType::DYNAMIC);
    spaceShip->assign<CircleCollision>(5);
}

void SpaceShipFactory::setBoundingCircle(EntityManager &entities, Entity *spaceShip) {
    Entity *boundingCircle = BoundingCircleFactory::create(entities, gameConfig.WARNING_DISTANCE);
    spaceShip->assign<Child>(boundingCircle);
}
