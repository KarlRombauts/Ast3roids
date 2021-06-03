//
// Created by Karl Rombauts on 13/3/21.
//

#ifndef UNTITLED_ENTITYMANAGER_H
#define UNTITLED_ENTITYMANAGER_H


#include <unordered_map>
#include <vector>
#include <Quaternion.h>
#include "Entity.h"
#include "../Vector3.h"
#include "../enum/TextAlignment.h"

class EntityManager {
private:
    unsigned int nextId;
    std::unordered_map<unsigned int, Entity*> entities;
public:
    Entity * create();

    Entity * createAsteroid(double radius);

    void destroy(Entity *entity);

    template<typename... Components>
    Entity *getFirstEntityWith();

    template<typename ... Components>
    std::vector<Entity*> getEntitiesWith();

    void createArena();

    Entity *createSpaceShip(Vector3 position);

    Entity *createBullet(Vector3 position, Vector3 velocity);

    Entity *createBoundingCircle(double radius);

    Entity *createBlackHole(double radius, Vector3 position);

    void destroyAll();

    void createWorld();

    Entity *createCamera(Vector3 position, Quaternion rotation);

    Entity *
    createGridPlane(Vector3 bottomLeft, Vector3 bottomRight, Vector3 topRight,
                    Vector3 topLeft);

    Entity *createSkybox();

    void createExplosion(const Vector3 &postion, double scale);
};

template<typename... Components>
std::vector<Entity*> EntityManager::getEntitiesWith() {
    std::vector<Entity*> result;

    for (auto entity: entities) {
        if (entity.second->has<Components...>()) {
           result.push_back(entity.second);
        }
    }

    return result;
}

template<typename... Components>
Entity* EntityManager::getFirstEntityWith() {
    std::vector<Entity*> result;

    for (auto entity: entities) {
        if (entity.second->has<Components...>()) {
            return entity.second;
        }
    }

    return nullptr;
}


#endif //UNTITLED_ENTITYMANAGER_H
