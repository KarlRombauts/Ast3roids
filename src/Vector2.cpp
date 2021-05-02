#include <cmath>
#include "Vector2.h"

Vector2::Vector2(double x, double y): x(x), y(y) {}

Vector2 Vector2::add(Vector2 vector) {
    double newX = x + vector.x;
    double newY = y + vector.y;
    return Vector2(newX, newY);
}

double Vector2::magnitude() {
    return sqrt(pow(x, 2) + pow(y, 2));
}

Vector2 Vector2::scale(double scalar) {
    double newX = x * scalar;
    double newY = y * scalar;
    return Vector2(newX, newY);
}

Vector2 Vector2::operator*(const double &scalar) {
    return scale(scalar);
}

Vector2 Vector2::operator/(const double &scalar) {
    return scale(1/scalar);
}

Vector2 Vector2::polar(double rotation, double radius) {
    double x = cos(rotation * (M_PI / 180)) * radius;
    double y = sin(rotation * (M_PI / 180)) * radius;
    return Vector2(x, y);
}

Vector2 Vector2::operator+(const Vector2 &vector) {
    return add(vector);
}

Vector2 Vector2::operator-(Vector2 vector) {
    return add(vector.scale(-1));
}

void Vector2::operator+=(const Vector2 &vector) {
    Vector2 newVector = *this + vector;
    x = newVector.x;
    y = newVector.y;
}

void Vector2::operator-=(const Vector2 &vector) {
    Vector2 newVector = *this - vector;
    x = newVector.x;
    y = newVector.y;
}

double Vector2::dot(Vector2 vector) {
    return x * vector.x + y * vector.y;
}

Vector2 Vector2::normalize() {
    return *this / magnitude();
}

Vector2 Vector2::perpendicular() {
    return Vector2(y, x * -1).normalize();
}

Vector2 Vector2::rotate(double theta) {
    double sinT = sin(theta);
    double cosT = cos(theta);
    return Vector2(x * cosT - y * sinT, x * sinT + y * cosT);
}

void Vector2::operator*=(const double &scalar) {
    const Vector2 &newVec = scale(scalar);
    x = newVec.x;
    y = newVec.y;
}

