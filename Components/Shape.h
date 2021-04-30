#ifndef UNTITLED_SHAPE_H
#define UNTITLED_SHAPE_H


#include <utility>
#include <vector>
#include "../ecs/Component.h"
#include "../Vec3.h"

struct Shape: public Component {
    Shape(std::vector<Vec3> vertices) : vertices(std::move(vertices)){}

    std::vector<Vec3> vertices;
};


#endif //UNTITLED_SHAPE_H
