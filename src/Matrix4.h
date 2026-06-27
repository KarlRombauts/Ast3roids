#ifndef STARFOX_MATRIX4_H
#define STARFOX_MATRIX4_H

#include "Vector3.h"

class Quaternion;

// A 4x4 matrix stored column-major (OpenGL layout: element at column c, row r is
// m[c * 4 + r]). This replaces the fixed-function matrix stack for the WebGL2 renderer.
class Matrix4 {
public:
    double m[16];

    Matrix4(); // zero matrix

    static Matrix4 identity();
    static Matrix4 translation(const Vector3 &t);
    static Matrix4 scale(const Vector3 &s);
    static Matrix4 perspective(double fovYDegrees, double aspect, double near, double far);
    static Matrix4 fromQuaternion(const Quaternion &q);

    Matrix4 operator*(const Matrix4 &rhs) const;

    // Transforms a point (w = 1), applying the perspective divide.
    Vector3 transformPoint(const Vector3 &p) const;

    const double *data() const { return m; }
};

#endif //STARFOX_MATRIX4_H
