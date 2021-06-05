#include <GameModel.h>
#include <Helpers.h>
#include <Globals.h>
#include <Components/Scale.h>
#include <Components/Asteroid.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Factory/Primatives/IcoSphere.h>
#include <Helpers/NoiseDistortion.h>
#include <Components/Collision.h>
#include <Components/Health.h>
#include <Components/Kinematics.h>
#include <Components/SplitOnDeath.h>
#include "AsteroidFactory.h"

Entity *
AsteroidFactory::create(EntityManager &entities, double radius) {
    Entity *asteroid = entities.create();
    asteroid->assign<Asteroid>(radius);
    asteroid->assign<Health>(radius * 10);

    // Assign Split Component to large asteroids
    if (radius > gameConfig.ASTEROID_MIN_SIZE) {
        asteroid->assign<SplitOnDeath>();
    }

    setTransformations(asteroid, radius);
    setGeometry(asteroid, radius);
    setCollisions(asteroid, radius);
    setKinematics(asteroid, radius);

    return asteroid;
}

void AsteroidFactory::setCollisions(Entity *asteroid, double radius) {
    asteroid->assign<Collision>(CollisionType::DYNAMIC);
    asteroid->assign<CircleCollision>(radius);
}

void AsteroidFactory::setKinematics(Entity *asteroid, double radius) {
    Kinematics kinematics;

    kinematics.velocity = Vector3::random(randf(10, 20));
    kinematics.mass = 4.0 / 3.0 * M_PI * pow(radius, 3);
    kinematics.angularVelocity = Vector3::random(
            randf(gameConfig.ASTEROID_MIN_ROTATION,
                  gameConfig.ASTEROID_MAX_ROTATION));

    asteroid->assign<Kinematics>(kinematics);
}

void AsteroidFactory::setGeometry(Entity *asteroid, double radius) {
    int subdivisions = getSubdivisions(radius);

    Geometry geometry = IcoSphere::create(subdivisions,
                                          materialLibrary.ASTEROID);

    distortMesh(geometry.vertices, 0.4, 0.5);

    asteroid->assign<Geometry>(geometry);
}

int AsteroidFactory::getSubdivisions(double radius) {
    int subdivisions;

    if (radius > 15) {
        subdivisions = 3;
    } else if (radius > 10) {
        subdivisions = 2;
    } else {
        subdivisions = 1;
    }
    return subdivisions;
}

double AsteroidFactory::setTransformations(Entity *asteroid, double radius) {
    double l = gameModel.arenaSize;
    asteroid->assign<Position>(Vector3(randf(-l,l), randf(-l,l), randf(-l,l)));

    asteroid->assign<Scale>(radius);
    asteroid->assign<Rotation>();
    return radius;
}
