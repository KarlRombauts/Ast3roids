#ifndef STARFOX_ANIMATEDTEXTURE_H
#define STARFOX_ANIMATEDTEXTURE_H

#include <ecs/Component.h>

enum class AnimationBehaviour {
    LOOP,
    DEATH,
};

struct AnimatedTexture : public Component {
    AnimatedTexture(int cols, int rows, AnimationBehaviour endBehaviour)
            : cols(cols), rows(rows), endBehaviour(endBehaviour) {}

    int rows;
    int cols;
    AnimationBehaviour endBehaviour;
    int rowOffset = 0;
    int colOffset = 0;
};


#endif //STARFOX_ANIMATEDTEXTURE_H
