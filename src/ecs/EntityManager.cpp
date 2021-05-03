#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/Plane.h>
#include <Components/PlaneCollision.h>
#include "EntityManager.h"
#include "Entity.h"
#include "../Components/Transform.h"
#include "../Components/Collision.h"
#include "../Components/Texture.h"
#include "../Components/Kinematics.h"
#include "../Components/Asteroid.h"
#include "../Helpers.h"
#include "../Components/Shape.h"
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
#include "../Components/Particle.h"
#include "../Components/Bullet.h"
#include "../Components/Damage.h"
#include "../Components/Rotation.h"


Entity *EntityManager::create() {
    unsigned int id = nextId;
    entities.insert(std::make_pair<int, Entity *>(nextId, new Entity(id)));
    nextId++;
    return entities.at(id);
}


Entity *EntityManager::createBlackHole(double radius, Vector3 position) {
    std::vector<Vector3> circle;
    int num_segments = 16;
    for (int i = 0; i < num_segments; i++) {
        double theta = 2.0 * M_PI * float(i) /
                       float(num_segments); // get the current angle
        double x = radius * cos(theta); // calculate the x component
        double y = radius * sin(theta); // calculate the y component
        Vector3 vertex = {x, y};
        circle.push_back(vertex);
    }


    Entity *blackHole = create();
    blackHole->assign<Transform>(position, 0, Vector3(1, 1));
    blackHole->assign<Shape>(circle);
    blackHole->assign<BlackHole>();
    blackHole->assign<Collision>(CollisionType::TRIGGER);
    blackHole->assign<CircleCollision>(radius / 4);
    blackHole->assign<GravityForce>(gameConfig.BLACK_HOLE_STRENGTH);
    blackHole->assign<Texture>(gameConfig.BLACK_HOLE_COLOR);
    return blackHole;
}


Entity *EntityManager::createAsteroid(double radius) {
    Entity *asteroid = this->create();
    const CoordinateSpace &world = gameModel.worldCoordinates;

    asteroid->assign<Transform>(
            Vector3(randf(world.minX, world.maxX),
                    randf(world.minY, world.maxY)),
            0, Vector3(1, 1));

    std::vector<Vector3> asteroidModel;
    int num_segments = 12;
    double roughness = 0.2;
    for (int i = 0; i < num_segments; i++) {
        double theta = 2.0 * M_PI * float(i) /
                       float(num_segments); // get the current angle
        double x = radius * cos(theta); // calculate the x component
        double y = radius * sin(theta); // calculate the y component

        Vector3 vertex = {
                x + radius * randf(-roughness, roughness),
                y + radius * randf(-roughness, roughness)
        };

        asteroidModel.push_back(vertex);
    }

    asteroid->assign<Asteroid>(radius);
    asteroid->assign<Shape>(asteroidModel);
    asteroid->assign<Collision>(CollisionType::DYNAMIC);
    asteroid->assign<CircleCollision>(radius);
    asteroid->assign<Texture>(gameConfig.ASTEROID_COLOR);
    asteroid->assign<Health>(radius * 10);

    Kinematics kinematics(Vector3::polar(randf(0, 360), randf(10, 20)),
                          Vector3(0, 0),
                          pow(radius, 2));
    kinematics.angularVelocity = randf(gameConfig.ASTEROID_MIN_ROTATION,
                                       gameConfig.ASTEROID_MAX_ROTATION);

    asteroid->assign<Kinematics>(kinematics);

    if (radius > 2) {
        asteroid->assign<SplitOnDeath>();
        asteroid->assign<HealthBar>();
    }

    return asteroid;
}

Entity *EntityManager::createFixedLine(Vector3 start, Vector3 end) {
    Entity *line = create();
    line->assign<Line>(start, end);
    line->assign<Collision>(CollisionType::STATIC);
    line->assign<LineCollision>(line->get<Line>());
    line->assign<Transform>();
    line->assign<Texture>(1, 1, 1);
    return line;
}


Entity *EntityManager::createGridPlane(Vector3 bottomLeft, Vector3 bottomRight,
                                       Vector3 topRight, Vector3 topLeft) {
    Entity *plane = create();
    plane->assign<Plane>(bottomLeft, bottomRight, topRight, topLeft);
    plane->assign<Collision>(CollisionType::STATIC);
    plane->assign<PlaneCollision>(plane->get<Plane>());
    plane->assign<Transform>();
    plane->assign<Rotation>();
    plane->assign<Texture>(1, 1, 1);
    return plane;
}

void EntityManager::createArena() {
    double l = gameModel.arenaSize;
    Entity *walls[6];

    // Front Wall
    walls[0] = createGridPlane({l, -l, -l}, {l, -l, l}, {l, l, l}, {l, l, -l});

    // Back Wall
    walls[1] = createGridPlane({-l, -l, -l}, {-l, -l, l}, {-l, l, l}, {-l, l, -l});

    // Left Wall
    walls[2] = createGridPlane({-l, -l, l}, {-l, l, l}, {l, l, l}, {l, -l, l});

    // Right Wall
    walls[3] = createGridPlane({-l, -l, -l}, {-l, l, -l}, {l, l, -l}, {l, -l, -l});

    // Top Wall
    walls[4] = createGridPlane({-l, l, -l}, {-l, l, l}, {l, l, l}, {l, l, -l});

    // Bottom Wall
    walls[5] = createGridPlane({-l, -l, -l}, {-l, -l, l}, {l, -l, l}, {l, -l, -l});

    for (Entity *wall: walls) {
        wall->assign<Wall>();
        wall->assign<Texture>(0.3, 0.3, 0.3);
    }
}

Entity *EntityManager::createSpaceShip(Vector3 position) {
    Entity *spaceShip = create();

    std::vector<Vector3> spaceShipModel = {
            {-1, -1, -1},
            {-1, -1, 1}
    };

    spaceShip->assign<Shape>(spaceShipModel);
    spaceShip->assign<SpaceShip>(gameConfig.PLAYER_FIRING_RATE,
                                 gameConfig.PLAYER_SPEED);
    spaceShip->assign<Collision>(CollisionType::DYNAMIC);
    spaceShip->assign<CircleCollision>(5);

    spaceShip->assign<Texture>(1, 0, 0);
    spaceShip->assign<Transform>(position, 90, Vector3(2, 2, 2));
    spaceShip->assign<Rotation>();
    spaceShip->assign<Kinematics>(Vector3(0, 0), Vector3(0, 0), 1);
    spaceShip->get<Kinematics>()->drag = 1;

    spaceShip->assign<PlayerInput>();

    Entity *boundingCircle = createBoundingCircle(gameConfig.WARNING_DISTANCE);
    spaceShip->assign<Child>(boundingCircle);
    return spaceShip;
}

Entity *EntityManager::createBoundingCircle(double radius) {
    Entity *boundingCircle = create();
    boundingCircle->assign<BoundingCircle>();
    boundingCircle->assign<Collision>(CollisionType::TRIGGER);
    boundingCircle->assign<CircleCollision>(radius);
    boundingCircle->assign<Transform>(Vector3(0, 0), 90, Vector3(1, 1));
    return boundingCircle;
}

void EntityManager::destroy(Entity *entity) {
    entities.erase(entity->getId());
    delete entity;
}

void EntityManager::createWorld() {
    createArena();
//    const Vector3 shipPosition = Vector3(gameModel.arenaSize * -0.7,
//                                   gameModel.arenaSize * -0.7);
    createCamera(Vector3(0, 0, 20), Quaternion());
    createSpaceShip(Vector3(0, 0, 0));


    Entity *center = create();
//    center->assign<Shape>(spaceShipModel);
    center->assign<Texture>(1, 0, 0);
    center->assign<Transform>(Vector3(0, 0, 0), 90, Vector3(2, 2, 2));
    center->assign<Rotation>();
    center->assign<Kinematics>(Vector3(0, 0), Vector3(0, 0), 1);
//    if (gameModel.difficulty == Difficulty::HARD) {
//        // Never create a black hole that is too close to the ship
//        int range = gameModel.arenaSize * 0.8;
//        Vector3 blackHolePosition = Vector3(0, 0);
//        do {
//            blackHolePosition = Vector3(randInt(-range, range),
//                                        randInt(-range, range));
//        } while ((blackHolePosition - shipPosition).magnitude() <
//                 gameModel.arenaSize * 0.4);
//
//        createBlackHole(10, blackHolePosition);
//    }
}

void EntityManager::destroyAll() {
    for (std::pair<const unsigned int, Entity *> entity: entities) {
        delete entity.second;
    }
    entities.clear();
}

Entity *EntityManager::createBullet(Vector3 position, Vector3 velocity) {
    Entity *bullet = create();
    bullet->assign<Transform>(position, 0, Vector3(1, 1));
    bullet->assign<Kinematics>(velocity, Vector3(0, 0), 1);
    bullet->assign<Particle>();
    bullet->assign<Bullet>();
    bullet->assign<Damage>(gameConfig.BULLET_DAMAGE);
    bullet->assign<Collision>(CollisionType::TRIGGER);
    bullet->assign<CircleCollision>(2);
    bullet->assign<Texture>(gameConfig.BULLET_COLOR);
    return bullet;
}

Entity *EntityManager::createCamera(Vector3 position, Quaternion rotation) {
    Entity *camera = create();
    double aspect = gameModel.height / gameModel.width;
    camera->assign<Camera>(60, aspect, 1, 1000);
    camera->assign<Position>(position);
    camera->assign<Rotation>(rotation);
}

