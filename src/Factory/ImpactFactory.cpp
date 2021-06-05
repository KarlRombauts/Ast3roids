#include <Components/Scale.h>
#include <Components/Rotation.h>
#include <Components/Position.h>
#include <Components/Transparency.h>
#include <Components/LookAt.h>
#include <GameModel.h>
#include <Globals.h>
#include <Factory/Primatives/PlaneFactory.h>
#include <Components/AnimatedTexture.h>
#include "ImpactFactory.h"

Entity *
ImpactFactory::create(EntityManager &entities, const Vector3 &position) {
    Entity *explosion = entities.create();

    setTransformations(explosion, position);
    setGeometry(explosion);

    return explosion;
}

void ImpactFactory::setTransformations(Entity *explosion, const Vector3 &position) {
    explosion->assign<Scale>(2);
    explosion->assign<Rotation>();
    explosion->assign<Position>(position);
    explosion->assign<LookAt>(gameModel.activeCamera);
}

void ImpactFactory::setGeometry(Entity *explosion) {
    explosion->assign<Geometry>(PlaneFactory::create(materialLibrary.IMPACT));
    explosion->assign<AnimatedTexture>(4, 3, AnimationBehaviour::DEATH);
    explosion->assign<Transparency>();
}
