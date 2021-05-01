#ifndef UNTITLED_CHILD_H
#define UNTITLED_CHILD_H


#include "../ecs/Entity.h"

struct Child : public Component {
    Child(Entity *child): child(child) {};
    Entity *child;
};


#endif //UNTITLED_CHILD_H
