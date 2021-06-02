#include <GLUT/glut.h>
#include <Components/Rotation.h>
#include "FiringSystem.h"
#include "../Components/FiringBullet.h"
#include "../Components/SpaceShip.h"
#include "../Components/Position.h"
#include "../Components/Kinematics.h"
#include "../Components/Color.h"
#include "../Globals.h"

void FiringSystem::update(EntityManager &entities, double dt) {
    for (Entity *entity: entities.getEntitiesWith<SpaceShip, FiringBullet>()) {
        SpaceShip* spaceShip = entity->get<SpaceShip>();

        int thisTime = glutGet(GLUT_ELAPSED_TIME);
        int timeSinceLastFire = thisTime - spaceShip->lastFire;

        if (timeSinceLastFire > spaceShip->fireRate) {
            spaceShip->lastFire = thisTime;
            entity->remove<FiringBullet>();

            Vector3 &position = entity->get<Position>()->position;
            Quaternion &rotation = entity->get<Rotation>()->rotation;

            // Todo: Rotate forward velocity by current rotation
            Vector3 bulletVelocity = entity->get<Kinematics>()->velocity + Vector3::polar(rotation, gameConfig.BULLET_SPEED);

            entities.createBullet(position, bulletVelocity);
        }
    }
}
