#include <Components/Position.h>
#include <Components/ParticleSource.h>
#include <Globals.h>
#include <GameModel.h>
#include <Components/Scale.h>
#include "ShipImpactSystem.h"
#include "../Components/SpaceShip.h"
#include "../Components/Impact.h"
#include "../Components/Asteroid.h"
#include "../Components/Wall.h"
#include "../Components/Destroy.h"

void ShipImpactSystem::update(EntityManager &entities) {
    for (Entity *spaceShip: entities.getEntitiesWith<SpaceShip, Impact>()) {
        for (Entity *otherEntity: spaceShip->get<Impact>()->entities) {
            if (otherEntity->hasAny<Asteroid, Wall>()) {
                spaceShip->remove<Scale>();
                entities.createExplosion(spaceShip->get<Position>()->position, 20);
                gameModel.state = GameState::GAME_OVER_TRANSITION;
                Entity* particles = entities.create();
//                    particles->assign<ParticleSource>(Vector3(0, 0, 0), 20, 100, gameConfig.EXPLOSION_DECAY_RATE);
                particles->assign<Position>(*spaceShip->get<Position>());
            }
        }
    }
}
