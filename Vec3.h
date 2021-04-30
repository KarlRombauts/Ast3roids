//
// Created by Karl Rombauts on 12/3/21.
//

#ifndef UNTITLED_VEC2_H
#define UNTITLED_VEC2_H

class Vec3 {
public:
    double y;
    double x;
    double z;

    Vec3(double x, double y);

    Vec3(double x, double y, double z);

    Vec3 add(Vec3 vector);

    double dot(Vec3 vector);

    Vec3 scale(double scalar);

    Vec3 normalize();

    double magnitude();

    Vec3 operator+(const Vec3 &vector);

    void operator+=(const Vec3 &vector);

    Vec3 operator-(Vec3 vector);

    void operator-=(const Vec3 &vector);

    void operator*=(const double &scalar);

    Vec3 operator*(const double &scalar);

    Vec3 operator/(const double &scalar);

    void operator/=(const double &scalar);

    static Vec3 polar(double rotation, double radius);

    Vec3 perpendicular();

    Vec3 rotate(double theta);

    void setXYZ(const Vec3 &newVec);
};

#endif //UNTITLED_VEC2_H
