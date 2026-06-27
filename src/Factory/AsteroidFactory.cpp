#include <GameModel.h>
#include <Helpers.h>
#include <Globals.h>
#include <Components/Scale.h>
#include <Components/Asteroid.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Factory/Primatives/UVSphere.h>
#include <Helpers/NoiseDistortion.h>
#include <Helpers/Normals.h>
#include <map>
#include <tuple>
#include <cmath>
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

// Average normals across vertices that share a position. The UV sphere
// duplicates its seam column and pole vertices (for the UVs); without this they
// each average only half their triangle fan and leave a lighting crease.
static void weldNormals(Geometry &geometry) {
    if (geometry.normals.size() != geometry.vertices.size()) {
        return;
    }
    auto key = [](const Vector3 &p) {
        return std::make_tuple((long long) std::llround(p.x * 1000),
                               (long long) std::llround(p.y * 1000),
                               (long long) std::llround(p.z * 1000));
    };
    std::map<std::tuple<long long, long long, long long>, Vector3> summed;
    for (size_t i = 0; i < geometry.vertices.size(); i++) {
        Vector3 &acc = summed[key(geometry.vertices[i])];
        acc.x += geometry.normals[i].x;
        acc.y += geometry.normals[i].y;
        acc.z += geometry.normals[i].z;
    }
    for (size_t i = 0; i < geometry.vertices.size(); i++) {
        geometry.normals[i] = summed[key(geometry.vertices[i])].normalize();
    }
}

void AsteroidFactory::setGeometry(Entity *asteroid, double radius) {
    int detail = getSubdivisions(radius);
    int rings = detail * 28;
    int segments = detail * 56;

    Geometry geometry = UVSphere::create(rings, segments, materialLibrary.ASTEROID);

    // Layered Perlin displacement at increasing frequency: big lumps, then
    // medium bumps, then fine surface roughness.
    distortMesh(geometry.vertices, 0.40, 0.5);
    distortMesh(geometry.vertices, 0.18, 1.4);
    distortMesh(geometry.vertices, 0.07, 2.8);

    // Recompute normals so the lit surface follows the new bumps, then weld the
    // duplicated seam/pole vertices back together.
    Normals::recalculate(geometry);
    weldNormals(geometry);

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
