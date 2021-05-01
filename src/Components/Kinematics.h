#ifndef UNTITLED_KINEMATICS_H
#define UNTITLED_KINEMATICS_H


#include "../Vector3.h"

struct Kinematics : public Component {
    Kinematics(const Vector3 &velocity, const Vector3 &acceleration, double mass) : velocity(
            velocity), acceleration(acceleration), mass(mass), angularVelocity(0), drag(0) {}

    Vector3 velocity;
    Vector3 acceleration;
    double angularVelocity;
    double drag;
    double mass;
};


#endif //UNTITLED_KINEMATICS_H
