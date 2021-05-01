#include "Entity.h"

Entity::Entity(unsigned int id)
        : id(id), components(std::unordered_map<std::type_index, Component *>()) {}

unsigned int Entity::getId() {
    return id;
}

Entity::~Entity() {
    for (std::pair<const std::type_index, Component *> component: components) {
        delete component.second;
    }
}

