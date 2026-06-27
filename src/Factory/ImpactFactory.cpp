#include <Components/Scale.h>
#include <Components/Rotation.h>
#include <Components/Position.h>
#include <Components/Transparency.h>
#include <Components/Additive.h>
#include <Components/LookAt.h>
#include <GameModel.h>
#include <Globals.h>
#include <Helpers.h>
#include <cmath>
#include <Factory/Primatives/PlaneFactory.h>
#include <Components/AnimatedTexture.h>
#include "ImpactFactory.h"

// How big the hit sprite is, and how far it is nudged toward the camera so the
// flat billboard sits in front of the (curved) asteroid surface instead of
// clipping into it.
static const double IMPACT_SCALE = 5.0;
static const double IMPACT_PUSH = 4.0;

Entity *
ImpactFactory::create(EntityManager &entities, const Vector3 &position) {
    Entity *impact = entities.create();

    // Nudge the sprite toward the camera along the impact->camera axis.
    Vector3 spawnPos = position;
    Entity *camera = gameModel.activeCamera;
    if (camera != nullptr && camera->has<Position>()) {
        Vector3 camPos = camera->get<Position>()->position;
        Vector3 toCamera = Vector3::fromTo(spawnPos, camPos).normalize();
        spawnPos = spawnPos + toCamera.scale(IMPACT_PUSH);
    }

    setTransformations(impact, spawnPos);
    setGeometry(impact);

    return impact;
}

void ImpactFactory::setTransformations(Entity *impact, const Vector3 &position) {
    double roll = randf(0, 2 * M_PI); // random in-plane spin so hits look varied

    impact->assign<Scale>(IMPACT_SCALE);
    impact->assign<Rotation>();
    impact->assign<Position>(position);
    impact->assign<LookAt>(gameModel.activeCamera, roll);
}

void ImpactFactory::setGeometry(Entity *impact) {
    impact->assign<Geometry>(PlaneFactory::create(materialLibrary.IMPACT));
    impact->assign<AnimatedTexture>(4, 3, AnimationBehaviour::DEATH);
    impact->assign<Transparency>();
    impact->assign<Additive>();
}
