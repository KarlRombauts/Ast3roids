#ifndef STARFOX_EXPLOSIONFACTORY_H
#define STARFOX_EXPLOSIONFACTORY_H


#include <ecs/Entity.h>
#include <ecs/EntityManager.h>

class ExplosionFactory {

    static void setLight(Entity *explosion);

    static void setGeometry(Entity *explosion);

    static void setTransformations(Entity *explosion, const Vector3 &position,
                       double scale);

public:
    static Entity * create(EntityManager &entities, const Vector3 &position, double scale);
};


#endif //STARFOX_EXPLOSIONFACTORY_H
