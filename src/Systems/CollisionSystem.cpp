#include <Components/BoundingCircle.h>
#include "CollisionSystem.h"
#include "../Components/Position.h"
#include "../Globals.h"
#include "../Components/Collision.h"
#include "../Components/Asteroid.h"
#include "../Components/SpaceShip.h"
#include "../Components/Bullet.h"
#include "../Components/Kinematics.h"
#include "../Components/BlackHole.h"
#include "../GameModel.h"
#include "../Components/Impact.h"
#include "../Components/OutsideArena.h"
#include "../Components/Particle.h"

void CollisionSystem::update(EntityManager &entities, double dt) {
    std::vector<Entity *> rigidBodyEntities = entities.getEntitiesWith<Position, Collision>();

    for (int i = 0; i < rigidBodyEntities.size(); i++) {
        Entity *entity1 = rigidBodyEntities.at(i);

        if (entity1->has<CircleCollision>() && intersectingWithArenaWall(entity1)) {
            handleArenaWallCollision(entity1);
        }

        for (int j = i + 1; j < rigidBodyEntities.size(); j++) {
            Entity *entity2 = rigidBodyEntities.at(j);

            if (entity1->has<CircleCollision>() &&
                entity2->has<CircleCollision>()) {

                if (entity1->has<Particle>() && entity2->has<Particle>()) {
                    continue;
                }

                if (areSpheresIntersecting(entity1, entity2)) {
                    createImpacts(entity1, entity2);
                    resolveCircleCircleCollision(entities, entity1, entity2);
                }
            }

//            if (entity1->has<CircleCollision>() &&
//                entity2->has<LineCollision>()) {
//                if (entity1->has<OutsideArena, CircleCollision>()) {
//                    Vector3 p = entity1->get<Position>()->position;
//                    double r = entity1->get<CircleCollision>()->radius;
//                    if (gameModel.isSphereFullyInsideArena(p, r)) {
//                        entity1->remove<OutsideArena>();
//                    }
//                    continue;
//                }
//
//                if (areCircleAndLineIntersecting(entity1, entity2)) {
//                    createImpacts(entity1, entity2);
//                    resolveCircleLineCollision(entity1, entity2);
//                }
//            }
        }
    }
}

void CollisionSystem::createImpacts(Entity *entity1, Entity *entity2) const {
    if(!entity1->has<Impact>()) {
       entity1->assign<Impact>();
    }
    if(!entity2->has<Impact>()) {
        entity2->assign<Impact>();
    }
    entity1->get<Impact>()->entities.push_back(entity2);
    entity2->get<Impact>()->entities.push_back(entity1);

    if (entity1->has<BlackHole>()) {
        std::cout << "Particle" << std::endl;
    }
}

const bool
CollisionSystem::areCircleAndLineIntersecting(Entity *circle,
                                              Entity *line) const {
    Vector3 q = circle->get<Position>()->position;
    double r = circle->get<CircleCollision>()->radius;


    Vector3 p1 = line->get<LineCollision>()->line->start;
    Vector3 p2 = line->get<LineCollision>()->line->end;
    Vector3 v = p2 - p1;

    // Solve the quadratic equation for solutions of circle intersection
    double a = v.dot(v);
    double b = 2 * v.dot(p1 - q);
    double c = p1.dot(p1) + q.dot(q) - 2 * p1.dot(q) - pow(r, 2);

    double discriminant = pow(b, 2) - 4 * a * c;

    // if discriminant < 0, then there are no real solutions
    if (discriminant < 0) return false;

    double sqrt_disc = sqrt(discriminant);
    double t1 = (-b + sqrt_disc) / (2 * a);
    double t2 = (-b - sqrt_disc) / (2 * a);

    return (t1 >= 0 || t1 <= 1) && (t2 >= 0 || t2 <= 1);
}

void CollisionSystem::resolveCircleLineCollision(Entity *circle,
                                                 Entity *line) const {
    Vector3 q = circle->get<Position>()->position;
    double r = circle->get<CircleCollision>()->radius;

    Vector3 p1 = line->get<LineCollision>()->line->start;
    Vector3 p2 = line->get<LineCollision>()->line->end;
    Vector3 v = p2 - p1;

    double t = fmax(0, fmin(1, -v.dot(p1 - q) / v.dot(v)));
    Vector3 closestPoint = p1 + v * t;

    Vector3 distance = q - closestPoint;

    if (circle->has<Kinematics, Position>() && circle->get<Collision>()->type == CollisionType::DYNAMIC) {
        // Static resolution
        Vector3 normal = distance.normalize();
        circle->get<Position>()->position += normal * r - distance;

        // Dynamic resolution
        Vector3 velocity = circle->get<Kinematics>()->velocity;
        circle->get<Kinematics>()->velocity =
                velocity - normal * 2 * (velocity.dot(normal));
    }
}


bool CollisionSystem::areSpheresIntersecting(Entity *entity1, Entity *entity2) const {
    Vector3 &position1 = entity1->get<Position>()->position;
    Vector3 &position2 = entity2->get<Position>()->position;
    double dist = Vector3::distanceBetween(position1, position2);
    
    double radius1 = entity1->get<CircleCollision>()->radius;
    double radius2 = entity2->get<CircleCollision>()->radius;
    return dist < radius1 + radius2;
}


void CollisionSystem::resolveCircleCircleCollision(
        EntityManager &entities, Entity *entity1, Entity *entity2) {

    if (entity1->has<Asteroid>() && entity2->has<Asteroid>()) {

        double r1 = entity1->get<CircleCollision>()->radius;
        double r2 = entity2->get<CircleCollision>()->radius;

        Vector3 p1 = entity1->get<Position>()->position;
        Vector3 p2 = entity2->get<Position>()->position;

        double m1 = entity1->get<Kinematics>()->mass;
        double m2 = entity2->get<Kinematics>()->mass;

        Vector3 v1 = entity1->get<Kinematics>()->velocity;
        Vector3 v2 = entity2->get<Kinematics>()->velocity;

        Vector3 diff = p1 - p2;
        double overlap = diff.magnitude() - (r1 + r2);

        Vector3 offset = (diff / diff.magnitude()) * overlap;


        // STATIC COLLISION RESOLUTION
        // both entities are moved so that they no longer overlap. The amount
        // they move is proportional to the mass of the other entity. This better
        // simulates conservation of momentum.

        p1 -= offset * (m2 / (m1 + m2));
        p2 += offset * (m1 / (m1 + m2));

        // DYNAMIC COLLISION RESOLUTION
        // Equation sourced from: https://en.wikipedia.org/wiki/Elastic_collision

        entity1->get<Kinematics>()->velocity = v1 - ((p1 - p2) * (v1 - v2).dot(p1 - p2) /
                                                     pow((p1 - p2).magnitude(), 2)) *
                                                    (2 * m2) / (m1 + m2);

        entity2->get<Kinematics>()->velocity = v2 - ((p2 - p1) * (v2 - v1).dot(p2 - p1) /
                                                     pow((p2 - p1).magnitude(), 2)) *
                                                    (2 * m1) / (m1 + m2);
    }
}

bool CollisionSystem::intersectingWithArenaWall(Entity *entity) {
    if (entity->has<OutsideArena>()) {
        return false;
    }

    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;
    return !gameModel.isSphereFullyInsideArena(position, radius);
}

void CollisionSystem::handleArenaWallCollision(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;
    Arena &arena = gameModel.arena;

    double length = gameModel.arenaSize - radius;

    if (entity->get<Collision>()->type == CollisionType::DYNAMIC) {
        //Static Resolution
        position = position.clampScalar(-length, length);

        // Dynamic Resolution
        if (entity->has<Kinematics>()) {
            Vector3 &velocity = entity->get<Kinematics>()->velocity;
            if (abs(position.x) >= length) {
                velocity.x *= -1;
            }

            if (abs(position.y) >= length) {
                velocity.y *= -1;
            }

            if (abs(position.z) >= length) {
                velocity.z *= -1;
            }
        }
    }

    createArenaImpacts(entity);
}

void CollisionSystem::createArenaImpacts(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    double radius = entity->get<CircleCollision>()->radius;
    Arena &arena = gameModel.arena;
    double length = gameModel.arenaSize - radius;

    if (entity->has<BoundingCircle>()) {

    }

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
