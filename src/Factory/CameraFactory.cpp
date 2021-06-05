//
// Created by Karl Rombauts on 5/6/21.
//

#include <Components/Rotation.h>
#include <GameModel.h>
#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/SmoothFollow.h>
#include "CameraFactory.h"

Entity * CameraFactory::create(EntityManager &entities, Entity *smoothFollowTarget) {
    Entity *camera = entities.create();
    camera->assign<Camera>(60, gameModel.aspectRatio, 1, 1000);
    camera->assign<Position>(Vector3(0, 5, 20));
    camera->assign<Rotation>();
    camera->assign<SmoothFollow>(smoothFollowTarget, Vector3(0, 5, 20));
    return camera;
}
