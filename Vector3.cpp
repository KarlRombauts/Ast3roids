#include <cmath>
#include "Vector3.h"

Vector3::Vector3(double x, double y) : x(x), y(y), z(1) {}

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3 Vector3::add(Vector3 vector) {
    double newX = x + vector.x;
    double newY = y + vector.y;
    double newZ = z + vector.z;
    return Vector3(newX, newY, newZ);
}

double Vector3::magnitude() {
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector3 Vector3::scale(double scalar) {
    double newX = x * scalar;
    double newY = y * scalar;
    double newZ = z * scalar;
    return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator*(const double &scalar) {
    return scale(scalar);
}

Vector3 Vector3::operator/(const double &scalar) {
    return scale(1 / scalar);
}

Vector3 Vector3::polar(double rotation, double radius) {
    double x = cos(rotation * (M_PI / 180)) * radius;
    double y = sin(rotation * (M_PI / 180)) * radius;
    return Vector3(x, y);
}

//Vector3 Vector3::polar(double rotation, double radius) {
//    double x = cos(rotation * (M_PI / 180)) * radius;
//    double y = sin(rotation * (M_PI / 180)) * radius;
//    return Vector3(x, y);
//}

Vector3 Vector3::operator+(const Vector3 &vector) {
    return add(vector);
}

Vector3 Vector3::operator-(Vector3 vector) {
    return add(vector.scale(-1));
}

void Vector3::operator+=(const Vector3 &vector) {
    Vector3 newVector = *this + vector;
    setXYZ(newVector);
}

void Vector3::operator-=(const Vector3 &vector) {
    Vector3 newVector = *this - vector;
    setXYZ(newVector);
}

double Vector3::dot(Vector3 vector) {
    return x * vector.x
           + y * vector.y
           + z * vector.z;
}

Vector3 Vector3::normalize() {
    return *this / magnitude();
}

Vector3 Vector3::perpendicular() {
    return Vector3(y, x * -1).normalize();
}

Vector3 Vector3::rotate(double theta) {
    double sinT = sin(theta);
    double cosT = cos(theta);
    return Vector3(x * cosT - y * sinT,
                x * sinT + y * cosT);
}

void Vector3::operator*=(const double &scalar) {
    const Vector3 &newVec = scale(scalar);
    setXYZ(newVec);
}

void Vector3::setXYZ(const Vector3 &newVec) {
    x = newVec.x;
    y = newVec.y;
    z = newVec.z;
}

void Vector3::operator/=(const double &scalar) {
    const Vector3 &newVec = *this / scalar;
    setXYZ(newVec);
}

