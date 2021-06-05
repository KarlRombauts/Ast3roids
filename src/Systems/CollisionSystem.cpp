#include "CollisionSystem.h"
#include "../Components/Position.h"
#include "../Components/Collision.h"
#include "../Components/Asteroid.h"
#include "../Components/Kinematics.h"
#include "../GameModel.h"
#include "../Components/Impact.h"
#include "../Components/OutsideArena.h"

void CollisionSystem::update(EntityManager &entities, double dt) {
    std::vector<Entity *> rigidBodyEntities = entities.getEntitiesWith<Position, Collision, CircleCollision>();

    for (int i = 0; i < rigidBodyEntities.size(); i++) {
        Entity *entity1 = rigidBodyEntities.at(i);

        if (intersectingWithArenaWall(entity1)) {
            createArenaImpacts(entity1);
            resolveArenaWallCollision(entity1);
        }

        for (int j = i + 1; j < rigidBodyEntities.size(); j++) {
            Entity *entity2 = rigidBodyEntities.at(j);

            if (areSpheresIntersecting(entity1, entity2)) {
                createImpacts(entity1, entity2);
                resolveCircleCircleCollision(entity1, entity2);
            }
        }
    }
}

void CollisionSystem::createImpacts(Entity *entity1, Entity *entity2) const {
    if (!entity1->has<Impact>()) {
        entity1->assign<Impact>();
    }

    if (!entity2->has<Impact>()) {
        entity2->assign<Impact>();
    }

    entity1->get<Impact>()->entities.push_back(entity2);
    entity2->get<Impact>()->entities.push_back(entity1);
}

bool CollisionSystem::areSpheresIntersecting(Entity *entity1, Entity *entity2) const {
    Vector3 &position1 = entity1->get<Position>()->position;
    Vector3 &position2 = entity2->get<Position>()->position;

    double dist = Vector3::distanceBetween(position1, position2);

    double radius1 = entity1->get<CircleCollision>()->radius;
    double radius2 = entity2->get<CircleCollision>()->radius;

    return dist < radius1 + radius2;
}

void CollisionSystem::resolveCircleCircleCollision(Entity *entity1, Entity *entity2) {

    if (entity1->has<Asteroid>() && entity2->has<Asteroid>()) {
        double r1 = entity1->get<CircleCollision>()->radius;
        double r2 = entity2->get<CircleCollision>()->radius;

        Vector3 p1 = entity1->get<Position>()->position;
        Vector3 p2 = entity2->get<Position>()->position;

        double m1 = entity1->get<Kinematics>()->mass;
        double m2 = entity2->get<Kinematics>()->mass;

        Vector3 v1 = entity1->get<Kinematics>()->velocity;
        Vector3 v2 = entity2->get<Kinematics>()->velocity;


        // STATIC COLLISION RESOLUTION
        // both entities are moved so that they no longer overlap. The amount
        // they move is proportional to the mass of the other entity. This better
        // simulates conservation of momentum.

        double overlap = Vector3::distanceBetween(p1, p2) - (r1 + r2);
        Vector3 offset = (p1 - p2).normalize() * overlap;

        entity1->get<Position>()->position -= offset * (m2 / (m1 + m2));
        entity2->get<Position>()->position += offset * (m1 / (m1 + m2));

        // DYNAMIC COLLISION RESOLUTION
        entity1->get<Kinematics>()->velocity = calculateNewVelocity(p1, p2, v1, v2, m1, m2);
        entity2->get<Kinematics>()->velocity = calculateNewVelocity(p2, p1, v2, v1, m2, m1);
    }
}

Vector3 CollisionSystem::calculateNewVelocity(Vector3 p1, Vector3 p2, Vector3 v1, Vector3 v2, double m1, double m2) const {
    // DYNAMIC COLLISION RESOLUTION
    // Equation sourced from: https://en.wikipedia.org/wiki/Elastic_collision

    return v1 - ((p1 - p2) * (v1 - v2).dot(p1 - p2) /
                 pow((p1 - p2).magnitude(), 2)) *
                (2 * m2) / (m1 + m2);
}

bool CollisionSystem::intersectingWithArenaWall(Entity *entity) {
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;

    if (entity->has<OutsideArena>()) {
        if (gameModel.isSphereFullyInsideArena(position, radius)) {
            entity->remove<OutsideArena>();
        }
        return false;
    }

    return !gameModel.isSphereFullyInsideArena(position, radius);
}

void CollisionSystem::resolveArenaWallCollision(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;
    Arena &arena = gameModel.arena;

    if (entity->get<Collision>()->type == CollisionType::DYNAMIC) {
        wallCollisionStaticResolution(position, radius);
        wallCollisionDynamicResolution(entity);
    }

}

void CollisionSystem::wallCollisionStaticResolution(Vector3 &position, double radius) const {
    double length = gameModel.arenaSize - radius;
    position = position.clampScalar(-length, length);
}

void CollisionSystem::wallCollisionDynamicResolution(Entity *entity) const {
    if (!entity->has<Kinematics>()) return;

    Vector3 &velocity = entity->get<Kinematics>()->velocity;
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;

    if (abs(position.x) + radius >= gameModel.arenaSize) {
        velocity.x *= -1;
    }

    if (abs(position.y) + radius >= gameModel.arenaSize) {
        velocity.y *= -1;
    }

    if (abs(position.z) + radius >= gameModel.arenaSize) {
        velocity.z *= -1;
    }
}

void CollisionSystem::createArenaImpacts(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;
    Arena &arena = gameModel.arena;
    double length = gameModel.arenaSize - radius;

    if (position.x >= length) {
        createImpacts(entity, arena.leftWall);
    }

    if (position.x <= -length) {
        createImpacts(entity, arena.rightWall);
    }

    if (position.y >= length) {
        createImpacts(entity, arena.topWall);
    }

    if (position.y <= -length) {
        createImpacts(entity, arena.bottomWall);
    }

    if (position.z >= length) {
        createImpacts(entity, arena.frontWall);
    }

    if (position.z <= -length) {
        createImpacts(entity, arena.backWall);
    }
}
