#include <sstream>
#include "Quaternion.h"

double *Quaternion::toRotationMatrix() {
    // Row 1
    double m00 = 2 * (w * w + x * x) - 1;
    double m01 = 2 * (x * y - w * z);
    double m02 = 2 * (x * z + w * y);

    // Row 2
    double m10 = 2 * (x * y + w * z);
    double m11 = 2 * (w * w + y * y) - 1;
    double m12 = 2 * (y * z - w * x);

    // Row 3
    double m20 = 2 * (x * z - w * y);
    double m21 = 2 * (y * z + w * x);
    double m22 = 2 * (w * w + z * z) - 1;

    return new double[16]{
            m00, m01, m02, 0,
            m10, m11, m12, 0,
            m20, m21, m22, 0,
            0, 0, 0, 1
    };
}

std::string Quaternion::toString() const {
    std::stringstream stringstream;
    stringstream << w << ", " << x << "i, " << y << "j, " << z << "k";
    return stringstream.str();
}

Quaternion Quaternion::operator*(const Quaternion &other) const {
    return Quaternion::multiply(other, *this);
}

void Quaternion::operator*=(const Quaternion &q) {
    *this = Quaternion::multiply(q, *this);
}

Quaternion Quaternion::identity() {
    return Quaternion();
}

Quaternion Quaternion::multiply(const Quaternion &a, const Quaternion &b) {
    double newX = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    double newY = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    double newZ = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    double newW = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return Quaternion(newX, newY, newZ, newW);
}

Quaternion Quaternion::fromTo(Vector3 from, Vector3 to) {
    double angle = Vector3::angle(from, to);
    Vector3 axis = from.cross(to);
    return Quaternion::angleAxis(angle, axis);
}

double Quaternion::getAngle() {
    return 2 * acos(w);
}

Vector3 Quaternion::getAxis() {
   return Vector3(x, y, z).normalize();
}

