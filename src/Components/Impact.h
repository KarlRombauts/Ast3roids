#ifndef UNTITLED_IMPACT_H
#define UNTITLED_IMPACT_H


#include <vector>
#include "../ecs/Entity.h"

struct Impact : public Component {
    std::vector<Entity*> entities;
};


#endif //UNTITLED_IMPACT_H
