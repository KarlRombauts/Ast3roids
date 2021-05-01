#ifndef UNTITLED_SHAPE_H
#define UNTITLED_SHAPE_H


#include <utility>
#include <vector>
#include "../ecs/Component.h"
#include "../Vector3.h"

struct Shape: public Component {
    Shape(std::vector<Vector3> vertices) : vertices(std::move(vertices)){}

    std::vector<Vector3> vertices;
};


#endif //UNTITLED_SHAPE_H
