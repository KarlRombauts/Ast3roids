#include <cmath>
#include "Vector3.h"
#include "Helpers.h"

Vector3::Vector3(double x, double y) : x(x), y(y), z(1) {}

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3 Vector3::random(double magnitude) {
    return Vector3(randf(-1, 1), randf(-1, 1), randf(-1, 1)).normalize() * magnitude;
}

Vector3 Vector3::add(Vector3 vector) {
    double newX = x + vector.x;
    double newY = y + vector.y;
    double newZ = z + vector.z;
    return Vector3(newX, newY, newZ);
}

double Vector3::magnitude() const {
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector3 Vector3::scale(double scalar) const {
    double newX = x * scalar;
    double newY = y * scalar;
    double newZ = z * scalar;
    return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator*(const double &scalar) const {
    return scale(scalar);
}

Vector3 Vector3::operator/(const double &scalar) const {
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

double Vector3::dot(const Vector3 &a, const Vector3 &b) {
    return a.x * b.x
           + a.y * b.y
           + a.z * b.z;
}

double Vector3::dot(const Vector3 &vector) const {
    return dot(*this, vector);
}

Vector3 Vector3::normalize() {
    return *this / magnitude();
}

Vector3 Vector3::perpendicular() const {
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

/**
 * Crosses this vector with the provided vector and returns the result
 * @param v the vector to cross with
 * @return the cross product (this × v)
 */
Vector3 Vector3::cross(const Vector3 &v) const {
    double newX = y * v.z - v.y * z;
    double newY = z * v.x - v.z * x;
    double newZ = x * v.y - v.x * y;

    return Vector3(newX, newY, newZ);
}

/**
 * Calculates the angle between two vectors. Specifically this is the angle that
 * rotates the from vector to the to vector along their cross product vector
 * @param from the starting vector
 * @param to the destination vector
 * @return the angle between the two vectors
 */
double Vector3::angle(const Vector3 &from, const Vector3 &to) {
    double adj = from.dot(to);
    double opp = (from.cross(to)).magnitude();
    return atan2(opp, adj);
}

Vector3 Vector3::lerp(const Vector3 &start, const Vector3 & end, double t) {
    Vector3 r;
    r.x = start.x + t * (end.x - start.x);
    r.y = start.y + t * (end.y - start.y);
    r.z = start.z + t * (end.z - start.z);

    return r;
}

bool Vector3::operator==(const Vector3 &other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3 &other) const {
    return x != other.x || y != other.y || z != other.z;
}

/**
 * Checks if the this vector (a) is lineally dependant on v. I.e. a = b*v where
 * b is some real number
 * @param v the vector to check
 * @return True if the two vectors are collinear, false otherwise.
 */
bool Vector3::isCollinear(Vector3 v) const {
    return (x * v.y - v.x * y) == 0 &&
           (x * v.z - v.x * z) == 0 &&
           (y * v.z - v.y * z) == 0;
}

bool Vector3::isNull() const {
    return x == 0 && y == 0 && z == 0;
}

/**
 * Return a vector that is orthogonal (perpendicular) to the given vector
 * @param v the supplied vector
 * @return the orthogonal vector
 */
Vector3 Vector3::orthogonalize(const Vector3 &v) {
    Vector3 right = v.cross(*this);
    return right.cross(v).normalize();
}

