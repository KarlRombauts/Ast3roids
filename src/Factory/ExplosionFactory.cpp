#include <Components/Light.h>
#include <Components/LightAnimation.h>
#include <Components/Scale.h>
#include <Components/Rotation.h>
#include <Components/Position.h>
#include <Components/Transparency.h>
#include <Components/LookAt.h>
#include <GameModel.h>
#include <Globals.h>
#include <Factory/Primatives/PlaneFactory.h>
#include <Components/AnimatedTexture.h>
#include "ExplosionFactory.h"

Entity *
ExplosionFactory::create(EntityManager &entities, const Vector3 &position,
                         double scale) {
    Entity *explosion = entities.create();

    setLight(explosion);
    setTransformations(explosion, position, scale);
    setGeometry(explosion);

    return explosion;
}

void ExplosionFactory::setTransformations(Entity *explosion, const Vector3 &position,
                                     double scale) {
    explosion->assign<Scale>(scale);
    explosion->assign<Rotation>();
    explosion->assign<Position>(position);
    explosion->assign<LookAt>(gameModel.activeCamera);
}

void ExplosionFactory::setGeometry(Entity *explosion) {
    Geometry geometry = PlaneFactory::create(materialLibrary.EXPLOSION);

    explosion->assign<AnimatedTexture>(5, 4, AnimationBehaviour::DEATH);

    explosion->assign<Geometry>(geometry);
    explosion->assign<Transparency>();
}

void ExplosionFactory::setLight(Entity *explosion) {
    Light startLight = Light(1, 0.5, 0.28); // Orange Light
    startLight.setAmbient(1, 0.5, 0.28);

    Light endLight = startLight;
    endLight.attenuation = 10;

    explosion->assign<Light>(startLight);
    explosion->assign<LightAnimation>(startLight, endLight, 30);
}
