#ifndef STARFOX_POSITION_H
#define STARFOX_POSITION_H


#include <Vector3.h>

struct Position: public Component{
    Position(): position(0, 0, 0) {};

    Position(Vector3 position): position(position) {};

    Vector3 position;
};

#endif //STARFOX_POSITION_H
