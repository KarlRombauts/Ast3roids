#include "CollisionSystem.h"
#include "../Components/Transform.h"
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
    std::vector<Entity *> rigidBodyEntities = entities.getEntitiesWith<Transform, Collision>();

    for (int i = 0; i < rigidBodyEntities.size(); i++) {
        Entity *entity1 = rigidBodyEntities.at(i);

        for (int j = i + 1; j < rigidBodyEntities.size(); j++) {
            Entity *entity2 = rigidBodyEntities.at(j);

            if (entity1->has<CircleCollision>() &&
                entity2->has<CircleCollision>()) {

                if (entity1->has<Particle>() && entity2->has<Particle>()) {
                    continue;
                }

                if (areCirclesIntersecting(entity1, entity2)) {
                    createImpacts(entity1, entity2);
                    resolveCircleCircleCollision(entities, entity1, entity2);
                }
            }

            if (entity1->has<CircleCollision>() &&
                entity2->has<LineCollision>()) {
                if (entity1->has<OutsideArena, CircleCollision>()) {
                    Vec3 p = entity1->get<Transform>()->position;
                    double r = entity1->get<CircleCollision>()->radius;
                    if (gameModel.isCircleInArena(p, r)) {
                        entity1->remove<OutsideArena>();
                    }
                    continue;
                }

                if (areCircleAndLineIntersecting(entity1, entity2)) {
                    createImpacts(entity1, entity2);
                    resolveCircleLineCollision(entity1, entity2);
                }
            }
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
    Vec3 q = circle->get<Transform>()->position;
    double r = circle->get<CircleCollision>()->radius;


    Vec3 p1 = line->get<LineCollision>()->line->start;
    Vec3 p2 = line->get<LineCollision>()->line->end;
    Vec3 v = p2 - p1;

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
    Vec3 q = circle->get<Transform>()->position;
    double r = circle->get<CircleCollision>()->radius;

    Vec3 p1 = line->get<LineCollision>()->line->start;
    Vec3 p2 = line->get<LineCollision>()->line->end;
    Vec3 v = p2 - p1;

    double t = fmax(0, fmin(1, -v.dot(p1 - q) / v.dot(v)));
    Vec3 closestPoint = p1 + v * t;

    Vec3 distance = q - closestPoint;

    if (circle->has<Kinematics, Transform>() && circle->get<Collision>()->type == CollisionType::DYNAMIC) {
        // Static resolution
        Vec3 normal = distance.normalize();
        circle->get<Transform>()->position += normal * r - distance;

        // Dynamic resolution
        Vec3 velocity = circle->get<Kinematics>()->velocity;
        circle->get<Kinematics>()->velocity =
                velocity - normal * 2 * (velocity.dot(normal));
    }
}


bool CollisionSystem::areCirclesIntersecting(Entity *entity1,
                                             Entity *entity2) const {
    Vec3 diff = (
            entity1->get<Transform>()->position -
            entity2->get<Transform>()->position
    );

    return diff.magnitude() < (entity1->get<CircleCollision>()->radius +
                               entity2->get<CircleCollision>()->radius);
}


void CollisionSystem::resolveCircleCircleCollision(
        EntityManager &entities, Entity *entity1, Entity *entity2) {

    if (entity1->has<Asteroid>() && entity2->has<Asteroid>()) {
        double r1 = entity1->get<CircleCollision>()->radius;
        double r2 = entity2->get<CircleCollision>()->radius;
        Transform *transform1 = entity1->get<Transform>();
        Transform *transform2 = entity2->get<Transform>();
        Kinematics *kinematics1 = entity1->get<Kinematics>();
        Kinematics *kinematics2 = entity2->get<Kinematics>();


        Vec3 diff = transform1->position - transform2->position;
        double overlap = diff.magnitude() - (r1 + r2);

        std::cout << "Asteroid collision: " << overlap << std::endl;

        Vec3 offset = (diff / diff.magnitude()) * overlap;

        double m1 = kinematics1->mass;
        double m2 = kinematics2->mass;

        Vec3 p1 = transform1->position;
        Vec3 p2 = transform2->position;
        Vec3 v1 = kinematics1->velocity;
        Vec3 v2 = kinematics2->velocity;

        // STATIC COLLISION RESOLUTION
        // both entities are moved so that they no longer overlap. The amount
        // they move is proportional to the mass of the other entity. This better
        // simulates conservation of momentum.

        transform1->position -= offset * (m2 / (m1 + m2));
        transform2->position += offset * (m1 / (m1 + m2));
//
//        // DYNAMIC COLLISION RESOLUTION
//        // Equation sourced from: https://en.wikipedia.org/wiki/Elastic_collision
//
        kinematics1->velocity = v1 - ((p1 - p2) * (v1 - v2).dot(p1 - p2) /
                                    pow((p1 - p2).magnitude(), 2)) *
                                   (2 * m2) / (m1 + m2);

        kinematics2->velocity = v2 - ((p2 - p1) * (v2 - v1).dot(p2 - p1) /
                                    pow((p2 - p1).magnitude(), 2)) *
                                   (2 * m1) / (m1 + m2);
    }
}
