#ifndef UNTITLED_VECTOR2_H
#define UNTITLED_VECTOR2_H

class Vector2 {
public:
    double y;
    double x;

    Vector2(): x(0), y(0) {};

    Vector2(double x, double y);

    Vector2 add(Vector2 vector);

    double dot(Vector2 vector);

    Vector2 scale(double scalar);

    Vector2 normalize();

    double magnitude();

    Vector2 operator+(const Vector2 &vector);

    void operator+=(const Vector2 &vector);

    Vector2 operator-(Vector2 vector);

    void operator-=(const Vector2 &vector);

    void operator*=(const double &scalar);

    Vector2 operator*(const double &scalar);

    Vector2 operator/(const double &scalar);


    static Vector2 polar(double rotation, double radius);

    Vector2 perpendicular();

    Vector2 rotate(double theta);
};

#endif //UNTITLED_VECTOR2_H
