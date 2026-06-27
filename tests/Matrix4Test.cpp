#include <Matrix4.h>
#include <Vector3.h>
#include <Quaternion.h>
#include "gtest/gtest.h"

static void expectVec3Near(const Vector3 &actual, const Vector3 &expected, double eps = 1e-9) {
    EXPECT_NEAR(actual.x, expected.x, eps);
    EXPECT_NEAR(actual.y, expected.y, eps);
    EXPECT_NEAR(actual.z, expected.z, eps);
}

TEST(Matrix4Test, Identity_LeavesPointUnchanged) {
    Vector3 p(3, -4, 5);
    expectVec3Near(Matrix4::identity().transformPoint(p), p);
}

TEST(Matrix4Test, Translation_MovesPoint) {
    Matrix4 t = Matrix4::translation(Vector3(1, 2, 3));
    expectVec3Near(t.transformPoint(Vector3(5, 5, 5)), Vector3(6, 7, 8));
}

TEST(Matrix4Test, Scale_ScalesPoint) {
    Matrix4 s = Matrix4::scale(Vector3(2, 3, 4));
    expectVec3Near(s.transformPoint(Vector3(1, 1, 1)), Vector3(2, 3, 4));
}

TEST(Matrix4Test, Multiply_AppliesRightmostFirst) {
    // M = translate * scale  =>  scale is applied first, then translate.
    Matrix4 m = Matrix4::translation(Vector3(1, 0, 0)) * Matrix4::scale(Vector3(2, 2, 2));
    expectVec3Near(m.transformPoint(Vector3(1, 1, 1)), Vector3(3, 2, 2));
}

TEST(Matrix4Test, Perspective_MapsNearPlaneToNdcMinusOne) {
    double near = 0.1, far = 100.0;
    Matrix4 p = Matrix4::perspective(60, 1.0, near, far);
    expectVec3Near(p.transformPoint(Vector3(0, 0, -near)), Vector3(0, 0, -1), 1e-6);
}

TEST(Matrix4Test, Perspective_MapsFarPlaneToNdcPlusOne) {
    double near = 0.1, far = 100.0;
    Matrix4 p = Matrix4::perspective(60, 1.0, near, far);
    expectVec3Near(p.transformPoint(Vector3(0, 0, -far)), Vector3(0, 0, 1), 1e-6);
}

TEST(Matrix4Test, FromQuaternion_Identity_LeavesPointUnchanged) {
    Vector3 p(1, 2, 3);
    expectVec3Near(Matrix4::fromQuaternion(Quaternion()).transformPoint(p), p);
}

TEST(Matrix4Test, FromQuaternion_90AboutZ_RotatesXToY) {
    Quaternion q = Quaternion::angleAxis(90, Vector3(0, 0, 1));
    expectVec3Near(Matrix4::fromQuaternion(q).transformPoint(Vector3(1, 0, 0)), Vector3(0, 1, 0));
}

TEST(Matrix4Test, FromQuaternion_90AboutY_RotatesXToNegativeZ) {
    Quaternion q = Quaternion::angleAxis(90, Vector3(0, 1, 0));
    expectVec3Near(Matrix4::fromQuaternion(q).transformPoint(Vector3(1, 0, 0)), Vector3(0, 0, -1));
}
