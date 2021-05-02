#include <sstream>
#include "Quaternion.h"
#include "Globals.h"

std::string Quaternion::toString() const {
    std::stringstream stringstream;
    stringstream << w << ", " << v.x << "i, " << v.y << "j, " << v.z << "k";
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
    Quaternion r;
    r.w = a.w * b.w - a.v.dot(b.v);
    r.v = a.v * b.w + b.v * a.w + a.v.cross(b.v);

    return r;
}

Quaternion Quaternion::fromTo(Vector3 from, Vector3 to) {
    double angle = Vector3::angle(from, to);
    Vector3 axis = from.cross(to);
    return Quaternion::angleAxis(angle * 180 / M_PI, axis);
}

double Quaternion::getAngle() {
    return 2 * acos(w);
}

Vector3 Quaternion::getAxis() {
    return v.normalize();
}

Quaternion Quaternion::slerp(Quaternion a, Quaternion b, double t) {
    double cosTheta = a.w * b.w + a.v.dot(b.v);

    // Always take the short path
    if (cosTheta < 0) {
        a.w = -a.w;
        a.v = a.v * -1;
        cosTheta = -cosTheta;
    }

    double k0, k1;

    // if a and b are very close, we just linearly interpolate
    if (cosTheta > 0.999) {
        k0 = 1 - t;
        k1 = t;
    } else {
        double sinTheta = sqrt(1 - pow(cosTheta, 2));
        double theta = atan2(sinTheta, cosTheta);

        k0 = sin((1 - t) * theta) / sinTheta;
        k1 = sin(t * theta) / sinTheta;
    }

    Quaternion r;
    r.w = a.w * k0 + b.w * k1;
    r.v = a.v * k0 + b.v * k1;

    return r;
}

Quaternion Quaternion::lookRotation(Vector3 forward, Vector3 up) {
    if (forward.isCollinear(up)) {
        return Quaternion::identity();
    }

    forward = forward.normalize();
    Vector3 desiredUp = up.orthogonalize(forward);

    Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

    Vector3 localUp = rot1 * Vector3::up();
    Quaternion rot2 = Quaternion::fromTo(localUp, desiredUp);

    return Quaternion::multiply(rot2, rot1);
}

Vector3 Quaternion::operator*(const Vector3 &vector) {
    Quaternion q = *this;
    Quaternion vectorQ = Quaternion(0, vector);
    Quaternion qc = this->conjugate();
    return Quaternion::multiply(Quaternion::multiply(q, vectorQ), qc).v;
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(w, v * -1);
}
