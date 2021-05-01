#ifndef UNTITLED_TEXTURE_H
#define UNTITLED_TEXTURE_H

#include "../ecs/Component.h"

struct Texture: public Component {
    Texture(double red, double green, double blue)
        : red(red), green(green), blue(blue) {}

    double red;
    double green;
    double blue;
};

#endif //UNTITLED_TEXTURE_H
