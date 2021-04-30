//
// Created by Karl Rombauts on 12/3/21.
//

#ifndef UNTITLED_VEC2_H
#define UNTITLED_VEC2_H

class Vector3 {
public:
    double y;
    double x;
    double z;

    Vector3(double x, double y);

    Vector3(double x, double y, double z);

    /**
     * Creates a unit vector pointing up
     * @return Vector3(0, 1, 0)
     */
    static Vector3 up() {
        return Vector3(0, 1, 0);
    }

    /**
     * Creates a unit vector pointing down
     * @return Vector3(0, -1, 0)
     */
    static Vector3 down() {
        return Vector3(0, -1, 0);
    }

    /**
     * Creates a unit vector pointing left
     * @return Vector3(-1, 0, 0)
     */
    static Vector3 left() {
        return Vector3(-1, 0, 0);
    }

    /**
     * Creates a unit vector pointing right
     * @return Vector3(1, 0, 0)
     */
    static Vector3 right() {
        return Vector3(1, 0, 0);
    }

    /**
     * Creates a unit vector pointing forward
     * @return Vector3(0, 0, 1)
     */
    static Vector3 forward() {
        return Vector3(0, 0, 1);
    }

    /**
     * Creates a unit vector pointing backwards
     * @return Vector3(0, 0, -1)
     */
    static Vector3 back() {
        return Vector3(0, 0, -1);
    }

    Vector3 add(Vector3 vector);

    double dot(Vector3 vector);

    Vector3 scale(double scalar);

    Vector3 normalize();

    double magnitude();

    Vector3 operator+(const Vector3 &vector);

    void operator+=(const Vector3 &vector);

    Vector3 operator-(Vector3 vector);

    void operator-=(const Vector3 &vector);

    void operator*=(const double &scalar);

    Vector3 operator*(const double &scalar);

    Vector3 operator/(const double &scalar);

    void operator/=(const double &scalar);

    static Vector3 polar(double rotation, double radius);

    Vector3 perpendicular();

    Vector3 rotate(double theta);

    void setXYZ(const Vector3 &newVec);
};

#endif //UNTITLED_VEC2_H
