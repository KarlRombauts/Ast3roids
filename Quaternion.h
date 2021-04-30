//
// Created by Karl Rombauts on 30/4/21.
//

#ifndef MAIN_QUATERNION_H
#define MAIN_QUATERNION_H


#include <cmath>
#include <iostream>
#include "Vector3.h"

class Quaternion {
public:
    double x;
    double y;
    double z;
    double w;

    Quaternion(double x, double y, double z, double w): x(x), y(y), z(z), w(w) {}

    static Quaternion angleAxis(double angle, Vector3 vector) {
        double radians = angle * (M_PI / 180);
        double w = cos(-radians / 2);

        Vector3 normVector = vector.normalize() * sin(-radians / 2);
        double x = normVector.x;
        double y = normVector.y;
        double z = normVector.z;

        return Quaternion(x, y, z, w);
    }

    static Quaternion identity();

    static Quaternion fromEuler(double yaw, double pitch, double roll);

    static Quaternion fromTo(Vector3 from, Vector3 to);

    static Quaternion lookRotation(Vector3 direction, Vector3 up);

    Quaternion slerp(Quaternion q, double t);

    Quaternion inverse();

    Quaternion multiply(const Quaternion &q) const;

    Quaternion operator*(const Quaternion &q) const;

    void operator*=(const Quaternion &q);


    Quaternion operator*(const Vector3 &q);

    Quaternion operator*=(const Vector3 &q);

    double * toRotationMatrix();

    std::string toString() const;
};


#endif //MAIN_QUATERNION_H
