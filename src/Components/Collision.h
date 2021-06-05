#ifndef UNTITLED_COLLISION_H
#define UNTITLED_COLLISION_H

#import <deque>
#include "Line.h"

enum class CollisionType {
    STATIC,
    DYNAMIC,
    TRIGGER
};

struct Collision: public Component {
    Collision(CollisionType type) : type(type) {}
    CollisionType type;
};

struct CircleCollision: public Component {
    double radius;
    CircleCollision(double radius) : radius(radius) {}
};

#endif //UNTITLED_COLLISION_H
