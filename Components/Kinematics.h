#ifndef UNTITLED_KINEMATICS_H
#define UNTITLED_KINEMATICS_H


#include "../Vec3.h"

struct Kinematics : public Component {
    Kinematics(const Vec3 &velocity, const Vec3 &acceleration, double mass) : velocity(
            velocity), acceleration(acceleration), mass(mass), angularVelocity(0), drag(0) {}

    Vec3 velocity;
    Vec3 acceleration;
    double angularVelocity;
    double drag;
    double mass;
};


#endif //UNTITLED_KINEMATICS_H
