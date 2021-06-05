#ifndef STARFOX_BOUNDINGCIRCLEFACTORY_H
#define STARFOX_BOUNDINGCIRCLEFACTORY_H


#include "ecs/EntityManager.h"
class BoundingCircleFactory {
public:
    static Entity * create(EntityManager &entities, double radius);
};


#endif //STARFOX_BOUNDINGCIRCLEFACTORY_H
