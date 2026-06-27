#include "Matrix4.h"
#include "Quaternion.h"
#include <cmath>

// Column-major storage: element at column c, row r is m[c * 4 + r].

Matrix4::Matrix4() {
    for (double &v : m) {
        v = 0;
    }
}

Matrix4 Matrix4::identity() {
    Matrix4 result;
    result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1;
    return result;
}

Matrix4 Matrix4::translation(const Vector3 &t) {
    Matrix4 result = identity();
    result.m[12] = t.x;
    result.m[13] = t.y;
    result.m[14] = t.z;
    return result;
}

Matrix4 Matrix4::scale(const Vector3 &s) {
    Matrix4 result;
    result.m[0] = s.x;
    result.m[5] = s.y;
    result.m[10] = s.z;
    result.m[15] = 1;
    return result;
}

Matrix4 Matrix4::perspective(double fovYDegrees, double aspect, double near, double far) {
    double f = 1.0 / std::tan((fovYDegrees * M_PI / 180.0) / 2.0);

    Matrix4 result;
    result.m[0] = f / aspect;
    result.m[5] = f;
    result.m[10] = (far + near) / (near - far);
    result.m[11] = -1;
    result.m[14] = (2 * far * near) / (near - far);
    return result;
}

Matrix4 Matrix4::fromQuaternion(const Quaternion &q) {
    double w = q.w, x = q.v.x, y = q.v.y, z = q.v.z;
    double sqw = w * w, sqx = x * x, sqy = y * y, sqz = z * z;

    Matrix4 result;
    // Column 0
    result.m[0] = sqw + sqx - sqy - sqz;
    result.m[1] = (2 * x * y) + (2 * w * z);
    result.m[2] = (2 * x * z) - (2 * w * y);
    // Column 1
    result.m[4] = (2 * x * y) - (2 * w * z);
    result.m[5] = sqw - sqx + sqy - sqz;
    result.m[6] = (2 * y * z) + (2 * w * x);
    // Column 2
    result.m[8] = (2 * x * z) + (2 * w * y);
    result.m[9] = (2 * y * z) - (2 * w * x);
    result.m[10] = sqw - sqx - sqy + sqz;
    // Column 3
    result.m[15] = 1;
    return result;
}

Matrix4 Matrix4::operator*(const Matrix4 &rhs) const {
    Matrix4 result;
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            double sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += m[k * 4 + r] * rhs.m[c * 4 + k];
            }
            result.m[c * 4 + r] = sum;
        }
    }
    return result;
}

Vector3 Matrix4::transformPoint(const Vector3 &p) const {
    double x = m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12];
    double y = m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13];
    double z = m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14];
    double w = m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15];

    if (w != 0 && w != 1) {
        x /= w;
        y /= w;
        z /= w;
    }
    return Vector3(x, y, z);
}
