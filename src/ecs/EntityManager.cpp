#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/Plane.h>
#include <Components/PlaneCollision.h>
#include <Factory/Primatives/IcoSphere.h>
#include <Helpers/ObjParser.h>
#include <Helpers/NoiseDistortion.h>
#include <Helpers/Normals.h>
#include <Components/Scale.h>
#include <Components/Skybox.h>
#include <Factory/Primatives/PlaneFactory.h>
#include <Components/LookAt.h>
#include <Components/Transparency.h>
#include <Components/AnimatedTexture.h>
#include <Components/Light.h>
#include <Components/LightAnimation.h>
#include <Factory/BoundingCircleFactory.h>
#include <Factory/ArenaFactory.h>
#include <Factory/SpaceShipFactory.h>
#include <Factory/CameraFactory.h>
#include "EntityManager.h"
#include "Entity.h"
#include "../Components/Position.h"
#include "../Components/Collision.h"
#include "../Components/Color.h"
#include "../Components/Kinematics.h"
#include "../Components/Asteroid.h"
#include "../Helpers.h"
#include "../GameModel.h"
#include "../Components/Health.h"
#include "../Components/HealthBar.h"
#include "../Components/SplitOnDeath.h"
#include "../Components/Wall.h"
#include "../Components/Child.h"
#include "../Components/BoundingCircle.h"
#include "../Components/PlayerInput.h"
#include "../Components/SpaceShip.h"
#include "../Components/BlackHole.h"
#include "../Components/GravityForce.h"
#include "../Globals.h"
#include "../Components/Bullet.h"
#include "../Components/Damage.h"
#include "../Components/Rotation.h"
#include "../Components/SmoothFollow.h"
#include "Factory/AsteroidFactory.h"


Entity *EntityManager::create() {
    unsigned int id = nextId;
    entities.emplace(id, new Entity(id));
    nextId++;
    return entities.at(id);
}

void EntityManager::destroy(Entity *entity) {
    entities.erase(entity->getId());
    delete entity;
}

void EntityManager::createWorld() {
    Entity *skybox = create();
    skybox->assign<Skybox>();
    skybox->assign<Geometry>(
            ObjParser().parse(gameConfig.MODEL_DIR + "/skybox/skybox.obj"));
    ArenaFactory::create(*this);

    Entity *spaceShip = SpaceShipFactory::create(*this, Vector3());

    Entity *camera = CameraFactory::create(*this, spaceShip);
    gameModel.activeCamera = camera;
    camera->assign<SmoothFollow>(spaceShip, Vector3(0, 5, 20));

    Entity *lightEntity = create();
    lightEntity->assign<Position>(Vector3(0, 0, -100));
    lightEntity->assign<Light>(0.2, 0.2, 0.2);

//    for (int i = 0; i < 30; i++) {
//        AsteroidFactory::create(*this, randf(6, 20));
//    }
}


void EntityManager::destroyAll() {
    for (std::pair<const unsigned int, Entity *> entity: entities) {
        delete entity.second;
    }
    entities.clear();
}

