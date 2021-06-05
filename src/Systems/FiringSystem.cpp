#include <GLUT/glut.h>
#include <Components/Rotation.h>
#include <Factory/BulletFactory.h>
#include "FiringSystem.h"
#include "../Components/FiringBullet.h"
#include "../Components/SpaceShip.h"
#include "../Components/Position.h"
#include "../Components/Kinematics.h"
#include "../Components/Color.h"
#include "../Globals.h"

void FiringSystem::update(EntityManager &entities, double dt) {
    for (Entity *spaceShip: entities.getEntitiesWith<SpaceShip, FiringBullet>()) {
        int &fireRate = spaceShip->get<SpaceShip>()->fireRate;
        int &lastFire = spaceShip->get<SpaceShip>()->lastFire;

        int thisTime = glutGet(GLUT_ELAPSED_TIME);

        int timeSinceLastFire = thisTime - lastFire;
        if (timeSinceLastFire > fireRate) {
            lastFire = thisTime;
            spaceShip->remove<FiringBullet>();

            Vector3 &shipPosition = spaceShip->get<Position>()->position;
            Quaternion &shipRotation = spaceShip->get<Rotation>()->rotation;
            Vector3 &shipVelocity = spaceShip->get<Kinematics>()->velocity;

            Vector3 bulletVelocity = shipVelocity + Vector3::polar(shipRotation, gameConfig.BULLET_SPEED);
            BulletFactory::create(entities, shipPosition, bulletVelocity);
        }
    }
}
