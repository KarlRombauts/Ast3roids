#ifndef UNTITLED_KINEMATICS_H
#define UNTITLED_KINEMATICS_H


#include "../Vector3.h"

struct Kinematics : public Component {
    Kinematics(const Vector3 &velocity, const Vector3 &acceleration, double mass)
            : velocity(velocity),
              acceleration(acceleration),
              angularVelocity(Vector3()),
              mass(mass),
              drag(0) {}

    Vector3 velocity;
    Vector3 angularVelocity;
    Vector3 acceleration;
    double drag;
    double mass;
};


#endif //UNTITLED_KINEMATICS_H
