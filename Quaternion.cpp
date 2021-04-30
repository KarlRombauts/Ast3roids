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

std::string Quaternion::toString() {
    std::stringstream stringstream;
    stringstream << w << ", " << x << "i, " << y << "j, " << z << "k";
    return stringstream.str();
}
