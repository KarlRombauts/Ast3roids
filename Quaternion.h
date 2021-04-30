//
// Created by Karl Rombauts on 30/4/21.
//

#ifndef MAIN_QUATERNION_H
#define MAIN_QUATERNION_H


#include <cmath>
#include <iostream>
#include "Vec3.h"

class Quaternion {
public:
    double x;
    double y;
    double z;
    double w;

    Quaternion(double x, double y, double z, double w): x(x), y(y), z(z), w(w) {}

    static Quaternion angleAxis(double angle, Vec3 vector) {
        double radians = angle * (M_PI / 180);
        double w = cos(-radians / 2);

        Vec3 normVector = vector.normalize() * sin(-radians / 2);
        double x = normVector.x;
        double y = normVector.y;
        double z = normVector.z;

        return Quaternion(x, y, z, w);
    }

    static Quaternion fromEuler(double yaw, double pitch, double roll);

    static Quaternion fromTo(Vec3 from, Vec3 to);

    static Quaternion lookRotation(Vec3 direction, Vec3 up);

    Quaternion slerp(Quaternion q, double t);

    Quaternion inverse();

    Quaternion mult(const Quaternion &q);

    Quaternion operator*(const Quaternion &q);

    Quaternion operator*=(const Quaternion &q);

    Quaternion mult(const Vec3 &v);

    Quaternion operator*(const Vec3 &q);

    Quaternion operator*=(const Vec3 &q);

    double * toRotationMatrix();

    std::string toString();
};


#endif //MAIN_QUATERNION_H
