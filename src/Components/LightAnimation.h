#ifndef STARFOX_LIGHTANIMATION_H
#define STARFOX_LIGHTANIMATION_H

#include "Light.h"

struct LightAnimation: public Component {
    LightAnimation(Light start, Light end, int duration)
            : start(start), end(end), duration(duration) {}

    Light start;
    Light end;
    int duration;
    int currentFrame = 0;
};


#endif //STARFOX_LIGHTANIMATION_H
