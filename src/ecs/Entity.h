#ifndef UNTITLED_ENTITY_H
#define UNTITLED_ENTITY_H

#include <utility>
#include <map>
#include <unordered_map>
#include <typeindex>
#include "Component.h"

class Entity {
private:
    unsigned int id;

    std::unordered_map<std::type_index, Component *> components;

public:
    explicit Entity(unsigned int id);
    ~Entity();

    template<typename Component, typename ... Args>
    void assign(Args &&... args);

    template<typename Component, typename ... Args>
    void remove();

    template<typename Component>
    Component * get();

    template<typename Component>
    bool has();

    // We need to specify both A and B so that the method is not ambiguous
    template<typename ComponentA, typename ComponentB, typename ... OtherComponents>
    bool has();

    unsigned int getId();

    template<typename ComponentA, typename ComponentB, typename ... OtherComponents>
    bool hasAny();

    template<typename Component>
    bool hasAny();
};

template<typename Component>
bool Entity::has() {
    return components.find(typeid(Component)) != components.end();
}

template<typename ComponentA, typename ComponentB, typename ... OtherComponents>
bool Entity::has() {
    return has<ComponentA>() && has<ComponentB, OtherComponents...>();
}

template<typename Component>
bool Entity::hasAny() {
    return has<Component>();
}

template<typename ComponentA, typename ComponentB, typename ... OtherComponents>
bool Entity::hasAny() {
    return has<ComponentA>() || hasAny<ComponentB, OtherComponents...>();
}

template<typename Component, typename... Args>
void Entity::assign(Args &&... args) {
    components[typeid(Component)] = static_cast<Component *>(new Component(
            std::forward<Args>(args) ...));
}

template<typename Component, typename... Args>
void Entity::remove() {
    Component *component = get<Component>();
    components.erase(typeid(Component));
    delete component;
}

template<typename Component>
Component * Entity::get() {
    return static_cast<Component*>(components.at(typeid(Component)));
}

#endif //UNTITLED_ENTITY_H
