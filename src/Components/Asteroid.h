#ifndef UNTITLED_ASTEROID_H
#define UNTITLED_ASTEROID_H

struct Asteroid : public Component {
    Asteroid(double size) : size(size) {}

    double size;
};

#endif //UNTITLED_ASTEROID_H
