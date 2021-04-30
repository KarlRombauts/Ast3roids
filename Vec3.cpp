#include <cmath>
#include "Vec3.h"

Vec3::Vec3(double x, double y) : x(x), y(y), z(1) {}

Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

Vec3 Vec3::add(Vec3 vector) {
    double newX = x + vector.x;
    double newY = y + vector.y;
    double newZ = z + vector.z;
    return Vec3(newX, newY, newZ);
}

double Vec3::magnitude() {
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vec3 Vec3::scale(double scalar) {
    double newX = x * scalar;
    double newY = y * scalar;
    double newZ = z * scalar;
    return Vec3(newX, newY, newZ);
}

Vec3 Vec3::operator*(const double &scalar) {
    return scale(scalar);
}

Vec3 Vec3::operator/(const double &scalar) {
    return scale(1 / scalar);
}

Vec3 Vec3::polar(double rotation, double radius) {
    double x = cos(rotation * (M_PI / 180)) * radius;
    double y = sin(rotation * (M_PI / 180)) * radius;
    return Vec3(x, y);
}

//Vec3 Vec3::polar(double rotation, double radius) {
//    double x = cos(rotation * (M_PI / 180)) * radius;
//    double y = sin(rotation * (M_PI / 180)) * radius;
//    return Vec3(x, y);
//}

Vec3 Vec3::operator+(const Vec3 &vector) {
    return add(vector);
}

Vec3 Vec3::operator-(Vec3 vector) {
    return add(vector.scale(-1));
}

void Vec3::operator+=(const Vec3 &vector) {
    Vec3 newVector = *this + vector;
    setXYZ(newVector);
}

void Vec3::operator-=(const Vec3 &vector) {
    Vec3 newVector = *this - vector;
    setXYZ(newVector);
}

double Vec3::dot(Vec3 vector) {
    return x * vector.x
           + y * vector.y
           + z * vector.z;
}

Vec3 Vec3::normalize() {
    return *this / magnitude();
}

Vec3 Vec3::perpendicular() {
    return Vec3(y, x * -1).normalize();
}

Vec3 Vec3::rotate(double theta) {
    double sinT = sin(theta);
    double cosT = cos(theta);
    return Vec3(x * cosT - y * sinT,
                x * sinT + y * cosT);
}

void Vec3::operator*=(const double &scalar) {
    const Vec3 &newVec = scale(scalar);
    setXYZ(newVec);
}

void Vec3::setXYZ(const Vec3 &newVec) {
    x = newVec.x;
    y = newVec.y;
    z = newVec.z;
}

void Vec3::operator/=(const double &scalar) {
    const Vec3 &newVec = *this / scalar;
    setXYZ(newVec);
}

