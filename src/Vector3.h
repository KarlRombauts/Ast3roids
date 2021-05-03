//
// Created by Karl Rombauts on 12/3/21.
//

#ifndef UNTITLED_VEC2_H
#define UNTITLED_VEC2_H

class Vector3 {
public:
    Vector3(): x(0), y(0), z(0) {};

    double y;
    double x;
    double z;

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
     * @return Vector3(1, 0, 0)
     */
    static Vector3 left() {
        return Vector3(1, 0, 0);
    }

    /**
     * Creates a unit vector pointing right
     * @return Vector3(-1, 0, 0)
     */
    static Vector3 right() {
        return Vector3(-1, 0, 0);
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

    Vector3 scale(double scalar) const;

    Vector3 normalize();

    double magnitude() const;

    Vector3 operator+(const Vector3 &vector);

    void operator+=(const Vector3 &vector);

    Vector3 operator-(Vector3 vector);

    void operator-=(const Vector3 &vector);

    void operator*=(const double &scalar);

    Vector3 operator*(const double &scalar) const;

    Vector3 operator/(const double &scalar) const;

    void operator/=(const double &scalar);

    bool operator==(const Vector3 &other) const;

    bool operator!=(const Vector3 &other) const;

    void setXYZ(const Vector3 &newVec);

    static double angle(const Vector3 &from, const Vector3 &to);

    bool isCollinear(Vector3 v) const;

    bool isNull() const;

    Vector3 orthogonalize(const Vector3 &v);

    static Vector3 lerp(const Vector3 &start, const Vector3 &end, double t);

    static Vector3 random(double magnitude);
};

#endif //UNTITLED_VEC2_H
