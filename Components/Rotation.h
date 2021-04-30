#ifndef MAIN_ROTATION_H
#define MAIN_ROTATION_H


#include "../Quaternion.h"
#include "../ecs/Component.h"

struct Rotation: Component {
    /**
     * Initialises the rotation component with an identity quaternion (no rotation)
     */
    Rotation(): rotation(Quaternion::identity()) {}

    /**
     * Initialises the rotation to a specific quaternion
     * @param rotation the quaternion that specifies the rotation
     */
    Rotation(Quaternion &rotation): rotation(rotation) {}

    Quaternion rotation;
};


#endif //MAIN_ROTATION_H
