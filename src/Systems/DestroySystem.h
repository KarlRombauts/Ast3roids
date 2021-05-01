//
// Created by Karl Rombauts on 18/4/21.
//

#ifndef UNTITLED_DESTROYSYSTEM_H
#define UNTITLED_DESTROYSYSTEM_H


#include "../ecs/EntityManager.h"
#include "../Components/Destroy.h"

class DestroySystem {
public:
    void update(EntityManager &entities);
};


#endif //UNTITLED_DESTROYSYSTEM_H
