#ifndef STARFOX_IMPACTFACTORY_H
#define STARFOX_IMPACTFACTORY_H

#include <ecs/Entity.h>
#include <ecs/EntityManager.h>
#include <Vector3.h>

class ImpactFactory {
public:
    static Entity *create(EntityManager &entities, const Vector3 &position);

private:
    static void setGeometry(Entity *impact);

    static void setTransformations(Entity *explosion, const Vector3 &position);
};


#endif //STARFOX_IMPACTFACTORY_H
