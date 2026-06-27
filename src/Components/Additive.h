#ifndef STARFOX_ADDITIVE_H
#define STARFOX_ADDITIVE_H


#include <ecs/Component.h>

// Marks a transparent entity to be drawn with additive blending so it glows
// against whatever is behind it (e.g. the hit sparks) instead of alpha-blending.
class Additive : public Component {};


#endif //STARFOX_ADDITIVE_H
