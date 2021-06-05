#include <Components/Plane.h>
#include <Components/Collision.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Scale.h>
#include <Components/Material.h>
#include <GameModel.h>
#include <Components/Wall.h>
#include "ArenaFactory.h"

Arena & ArenaFactory::create(EntityManager &entities) {
    Arena &arena = gameModel.arena;

    arena.leftWall = createWall(entities, ArenaWall::LEFT);
    arena.rightWall = createWall(entities, ArenaWall::RIGHT);
    arena.frontWall = createWall(entities, ArenaWall::FRONT);
    arena.backWall = createWall(entities, ArenaWall::BACK);
    arena.topWall = createWall(entities, ArenaWall::TOP);
    arena.bottomWall = createWall(entities, ArenaWall::BOTTOM);

    return arena;
}

Entity *ArenaFactory::createWall(EntityManager &entities, ArenaWall side) {
    double l = gameModel.arena.size;
    Entity *wall = entities.create();
    wall->assign<Wall>();

    switch (side) {
        case ArenaWall::TOP:
            return createGridPlane(wall, {-l, l, -l}, {-l, l, l}, {l, l, l}, {l, l, -l});
        case ArenaWall::BOTTOM:
            return createGridPlane(wall, {-l, -l, -l}, {-l, -l, l}, {l, -l, l}, {l, -l, -l});
        case ArenaWall::LEFT:
            return createGridPlane(wall, {l, -l, -l}, {l, -l, l}, {l, l, l}, {l, l, -l});
        case ArenaWall::RIGHT:
            return createGridPlane(wall, {-l, -l, -l}, {-l, -l, l}, {-l, l, l}, {-l, l, -l});
        case ArenaWall::FRONT:
            return createGridPlane(wall, {-l, -l, l}, {-l, l, l}, {l, l, l}, {l, -l, l});
        case ArenaWall::BACK:
            return createGridPlane(wall, {-l, -l, -l}, {-l, l, -l}, {l, l, -l}, {l, -l, -l});
    }
}



Entity *ArenaFactory::createGridPlane(Entity *plane, Vector3 bottomLeft, Vector3 bottomRight,
                                      Vector3 topRight, Vector3 topLeft) {

    plane->assign<Plane>(bottomLeft, bottomRight, topRight, topLeft);
    plane->assign<Collision>(CollisionType::STATIC);

    plane->assign<Position>();
    plane->assign<Rotation>();
    plane->assign<Scale>();

    plane->assign<Material>();
    return plane;
}