#ifndef UNTITLED_DAMAGE_H
#define UNTITLED_DAMAGE_H

#include "../ecs/Component.h"

struct Damage : public Component {
    Damage(int damage): damage(damage) {}
    int damage;
};

#endif //UNTITLED_DAMAGE_H
