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

    static double dot(const Vector3 &a, const Vector3 &b);

    double dot(const Vector3 &vector) const;

    Vector3 cross(const Vector3 &vector3) const;

    Vector3 scale(double scalar);

    Vector3 normalize();

    double magnitude() const;

    Vector3 operator+(const Vector3 &vector);

    void operator+=(const Vector3 &vector);

    Vector3 operator-(Vector3 vector);

    void operator-=(const Vector3 &vector);

    void operator*=(const double &scalar);

    Vector3 operator*(const double &scalar);

    Vector3 operator/(const double &scalar);

    void operator/=(const double &scalar);

    bool operator==(const Vector3 &other) const;

    bool operator!=(const Vector3 &other) const;

    static Vector3 polar(double rotation, double radius);

    Vector3 perpendicular() const;

    Vector3 rotate(double theta);

    void setXYZ(const Vector3 &newVec);

    static double angle(const Vector3 &from, const Vector3 &to);
};

#endif //UNTITLED_VEC2_H
