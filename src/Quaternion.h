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
    double w;
    Vector3 v;

    Quaternion(): w(1), v(Vector3(0, 0, 0)) {};

    Quaternion(double x, double y, double z, double w): w(w), v(x, y, z) {}

    Quaternion(double w, Vector3 v): w(w), v(v) {}

    static Quaternion angleAxis(double angle, Vector3 vector) {
        double radians = angle * (M_PI / 180);
        double w = cos(radians / 2);
        Vector3 v = vector.normalize() * sin(radians / 2);
        return Quaternion(w, v);
    }

    static Quaternion identity();

    static Quaternion fromTo(Vector3 from, Vector3 to);

    static Quaternion lookRotation(Vector3 forward, Vector3 up);

    double getAngle();

    static Quaternion slerp(Quaternion a, Quaternion b, double t);

    Quaternion conjugate() const;

    Quaternion static multiply(const Quaternion &a, const Quaternion &b);

    Quaternion operator*(const Quaternion &q) const;

    void operator*=(const Quaternion &q);

    Vector3 operator*(const Vector3 &vector);

    Quaternion operator*=(const Vector3 &q);

    std::string toString() const;

    Vector3 getAxis();
};


#endif //MAIN_QUATERNION_H
