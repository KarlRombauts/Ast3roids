#include <Components/Position.h>
#include "AsteroidSystem.h"
#include "../Globals.h"
#include "Factory/AsteroidFactory.h"

void AsteroidSystem::startWave(EntityManager &entities, int waveCount) {
    Entity *spaceShip = entities.getFirstEntityWith<SpaceShip>();

    for (int i = 0; i < waveCount; i++) {
        launchAsteroidAtSpaceShip(entities, spaceShip);
    }
}

void AsteroidSystem::launchAsteroidAtSpaceShip(EntityManager &entities, Entity *spaceShip) const {
    int radius = randInt(gameConfig.ASTEROID_MIN_START_RADIUS, gameConfig.ASTEROID_MAX_START_RADIUS);
    Entity *asteroid = AsteroidFactory::create(entities, radius);
    Vector3 asteroidPosition = Vector3::random(200);

    asteroid->assign<Position>(asteroidPosition);
    asteroid->assign<OutsideArena>();

    if (spaceShip) {
        asteroid->get<Kinematics>()->velocity = calculateAsteroidVelocity(spaceShip, asteroidPosition);
    }
}

Vector3 AsteroidSystem::calculateAsteroidVelocity(Entity *spaceShip, Vector3 &asteroidPosition) const {
    Vector3 &spaceShipPosition = spaceShip->get<Position>()->position;
    const Vector3 &direction = Vector3::fromTo(asteroidPosition, spaceShipPosition).normalize();

    double speed = randf(gameConfig.ASTEROID_MIN_SPEED, gameConfig.ASTEROID_MAX_SPEED);
    return direction.scale(speed);
}
