#include <Helpers.h>
#include "gtest/gtest.h"
#include "../src/Quaternion.h"

double error = 0.000000001;

TEST(QuaternionTest, InitialisesToIdentityQuaternion) {
    Quaternion q;
    EXPECT_EQ(q.v, Vector3(0, 0, 0));
    EXPECT_EQ(q.w, 1);
}

TEST(QuaternionTest, MultiplicationOfIdentityReturnsIdentity) {
    Quaternion a;
    Quaternion b;
    Quaternion q = a * b;

    EXPECT_NEAR(q.v.x, 0, error);
    EXPECT_NEAR(q.v.y, 0, error);
    EXPECT_NEAR(q.v.z, 0, error);
    EXPECT_NEAR(q.w, 1, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_NoRotation) {
    Quaternion q = Quaternion::angleAxis(0, Vector3::up());

    EXPECT_NEAR(q.v.x, 0, error);
    EXPECT_NEAR(q.v.y, 0, error);
    EXPECT_NEAR(q.v.z, 0, error);
    EXPECT_NEAR(q.w, 1, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_90_Degrees) {
    Quaternion q = Quaternion::angleAxis(90, Vector3::up());

    EXPECT_NEAR(q.v.x, 0, error);
    EXPECT_NEAR(q.v.y, 1 / sqrt(2), error);
    EXPECT_NEAR(q.v.z, 0, error);
    EXPECT_NEAR(q.w, 1 / sqrt(2), error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_180_Degrees) {
    Quaternion q = Quaternion::angleAxis(180, Vector3::up());
    EXPECT_NEAR(q.v.x, 0, error);
    EXPECT_NEAR(q.v.y, 1, error);
    EXPECT_NEAR(q.v.z, 0, error);
    EXPECT_NEAR(q.w, 0, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_270_Degrees) {
    Quaternion q = Quaternion::angleAxis(270, Vector3::up());
    EXPECT_NEAR(q.v.x, 0, error);
    EXPECT_NEAR(q.v.y, 1 / sqrt(2), error);
    EXPECT_NEAR(q.v.z, 0, error);
    EXPECT_NEAR(q.w, -1 / sqrt(2), error);
}

TEST(QuaternionTest, Slerp_1) {
    Quaternion a = Quaternion::angleAxis(0, Vector3::up());
    Quaternion b = Quaternion::angleAxis(180, Vector3::up());
    Quaternion q = Quaternion::slerp(a, b, 0.5);
    EXPECT_FLOAT_EQ(q.getAngle(), M_PI / 2);
}

TEST(QuaternionTest, Slerp_2) {
    Quaternion a = Quaternion::angleAxis(0, Vector3::up());
    Quaternion b = Quaternion::angleAxis(100, Vector3::up());
    Quaternion q = Quaternion::slerp(a, b, 0.2);
    EXPECT_FLOAT_EQ(q.getAngle(), 20 * M_PI / 180);
}

TEST(QuaternionTest, RotateVector) {
    Quaternion a = Quaternion::angleAxis(90, Vector3::up());
    Vector3 v(0, 1, 0);

    EXPECT_EQ(a * v, v);
}


TEST(QuaternionTest, RotateVector_1) {
    Quaternion a = Quaternion::angleAxis(90, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, 1, error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 0, error);
}

TEST(QuaternionTest, RotateVector_2) {
    Quaternion a = Quaternion::angleAxis(90, Vector3::up());
    Vector3 v(0, 0, 2);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, 2, error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 0, error);
}

TEST(QuaternionTest, RotateVector_3) {
    Quaternion a = Quaternion::angleAxis(45, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, 1 / sqrt(2), error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 1 / sqrt(2), error);
}

TEST(QuaternionTest, RotateVector_4) {
    Quaternion a = Quaternion::angleAxis(-45, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, -1 / sqrt(2), error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 1 / sqrt(2), error);
}

TEST(QuaternionTest, RotateVector_5) {
    Quaternion a = Quaternion::angleAxis(270, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, -1, error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 0, error);
}

TEST(QuaternionTest, RotateVector_6) {
    Quaternion a = Quaternion::angleAxis(180, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, 0, error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, -1, error);
}

TEST(QuaternionTest, RotateVector_7) {
    Quaternion a = Quaternion::angleAxis(360, Vector3::up());
    Vector3 v(0, 0, 1);

    const Vector3 &v2 = a * v;
    EXPECT_NEAR(v2.x, 0, error);
    EXPECT_NEAR(v2.y, 0, error);
    EXPECT_NEAR(v2.z, 1, error);
}


TEST(QuaternionTest, LookAt) {
    Vector3 forward = Vector3(2, 7, 9).normalize();

    Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

    Vector3 r = rot1 * Vector3::forward();
    EXPECT_NEAR(forward.x, r.x, error);
    EXPECT_NEAR(forward.y, r.y, error);
    EXPECT_NEAR(forward.z, r.z, error);
}

TEST(QuaternionTest, LookAt_2) {
    Vector3 forward = Vector3(-2, -7, -9).normalize();

    Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

    Vector3 r = rot1 * Vector3::forward();
    EXPECT_NEAR(forward.x, r.x, error);
    EXPECT_NEAR(forward.y, r.y, error);
    EXPECT_NEAR(forward.z, r.z, error);
}

TEST(QuaternionTest, LookAt_3) {
    for (int i = 0; i < 100000; i++) {
        Vector3 forward = Vector3(randf(-100, 100), randf(-100, 100),
                                  randf(-100, 100)).normalize();

        Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

        Vector3 r = rot1 * Vector3::forward();
        EXPECT_NEAR(forward.x, r.x, error);
        EXPECT_NEAR(forward.y, r.y, error);
        EXPECT_NEAR(forward.z, r.z, error);
    }

    for (int i = 0; i < 100000; i++) {
        Vector3 from = Vector3(randf(-100, 100), randf(-100, 100),
                               randf(-100, 100)).normalize();
        Vector3 to = Vector3(randf(-100, 100), randf(-100, 100),
                             randf(-100, 100)).normalize();

        Quaternion rot1 = Quaternion::fromTo(from, to);

        Vector3 r = rot1 * from;
        EXPECT_NEAR(r.x, to.x, error);
        EXPECT_NEAR(r.y, to.y, error);
        EXPECT_NEAR(r.z, to.z, error);
    }
}

TEST(QuaternionTest, LocalAxis_1) {
    Vector3 forward = Vector3::forward();

    Vector3 lookAt = Vector3(2, 7, 9);
    Vector3 lookAtNormal = lookAt.normalize();

    Quaternion rot1 = Quaternion::fromTo(forward, lookAt);
    Vector3 localLeft = rot1 * Vector3::left();
    Vector3 localUp = rot1 * Vector3::up();
    Vector3 localForward = rot1 * Vector3::forward();

    EXPECT_NEAR(localForward.x, lookAtNormal.x, error);
    EXPECT_NEAR(localForward.y, lookAtNormal.y, error);
    EXPECT_NEAR(localForward.z, lookAtNormal.z, error);

    EXPECT_NEAR(localLeft.dot(localUp), 0, error);
    EXPECT_NEAR(localLeft.dot(localForward), 0, error);
    EXPECT_NEAR(localForward.dot(localUp), 0, error);
}

TEST(QuaternionTest, LocalAxis_2) {
    for (int i = 0; i < 100000; i++) {
        Vector3 forward = Vector3::forward();

        Vector3 lookAt = Vector3(randf(-100, 100), randf(-100, 100),
                                 randf(-100, 100));
        Vector3 lookAtNormal = lookAt.normalize();

        Quaternion rot1 = Quaternion::fromTo(forward, lookAt);
        Vector3 localLeft = rot1 * Vector3::left();
        Vector3 localUp = rot1 * Vector3::up();
        Vector3 localForward = rot1 * Vector3::forward();

        EXPECT_NEAR(localForward.x, lookAtNormal.x, error);
        EXPECT_NEAR(localForward.y, lookAtNormal.y, error);
        EXPECT_NEAR(localForward.z, lookAtNormal.z, error);

        EXPECT_NEAR(localLeft.dot(localUp), 0, error);
        EXPECT_NEAR(localLeft.dot(localForward), 0, error);
        EXPECT_NEAR(localForward.dot(localUp), 0, error);
    }
}



TEST(QuaternionTest, LookAt_4) {
    for (int i = 0; i < 100000; i++) {
        Vector3 forward = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100)).normalize();
        Vector3 desiredUp = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100));
        desiredUp = desiredUp.orthogonalize(forward);

        Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

        Vector3 localUp = rot1 * Vector3::up();
        Quaternion rot2 = Quaternion::fromTo(localUp, desiredUp);

        Vector3 axis = rot2.v.normalize();

        EXPECT_NEAR(localUp.dot(forward), 0, error);
        EXPECT_NEAR(desiredUp.dot(forward), 0, error);
        EXPECT_NEAR(abs(forward.x), abs(axis.x), error);
        EXPECT_NEAR(abs(forward.y), abs(axis.y), error);
        EXPECT_NEAR(abs(forward.z), abs(axis.z), error);

        Quaternion targetRotation = Quaternion::multiply(rot2, rot1);

    }
}

TEST(QuaternionTest, LookAt_5) {
    for (int i = 0; i < 10000; i++) {
        Vector3 forward = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100)).normalize();
        Vector3 desiredUp = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100));
        desiredUp = desiredUp.orthogonalize(forward);

        Quaternion rot1 = Quaternion::fromTo(Vector3::forward(), forward);

        Vector3 localUp = rot1 * Vector3::up();
        Quaternion rot2 = Quaternion::fromTo(localUp, desiredUp);

        Quaternion targetRotation = Quaternion::multiply(rot2, rot1);

        Vector3 localForward = targetRotation * Vector3::forward();
        EXPECT_NEAR(localForward.x, forward.x, error);
        EXPECT_NEAR(localForward.y, forward.y, error);
        EXPECT_NEAR(localForward.z, forward.z, error);

        localUp = targetRotation * Vector3::up();
        EXPECT_NEAR(localUp.x, desiredUp.x, error);
        EXPECT_NEAR(localUp.y, desiredUp.y, error);
        EXPECT_NEAR(localUp.z, desiredUp.z, error);
    }
}

TEST(QuaternionTest, LookRotation_1) {
    for (int i = 0; i < 10000; i++) {
        Vector3 forward = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100)).normalize();
        Vector3 desiredUp = Vector3(randf(-100, 100), randf(-100, 100), randf(-100, 100));

        Quaternion targetRotation = Quaternion::lookRotation(forward, desiredUp);

        Vector3 localForward = targetRotation * Vector3::forward();
        EXPECT_NEAR(localForward.x, forward.x, error);
        EXPECT_NEAR(localForward.y, forward.y, error);
        EXPECT_NEAR(localForward.z, forward.z, error);

        Vector3 localUp = targetRotation * Vector3::up();
        Vector3 a = localUp.cross(desiredUp).normalize();
        Vector3 b = localForward.cross(desiredUp).normalize();

        EXPECT_NEAR(abs(a.x), abs(b.x), error);
        EXPECT_NEAR(abs(a.y), abs(b.y), error);
        EXPECT_NEAR(abs(a.z), abs(b.z), error);
    }
}

TEST(QuaternionTest, Normalise) {
    for (int i = 0; i < 10000; i++) {
        Quaternion q = Quaternion::angleAxis(randf(0, 360), Vector3::random(randf(1, 20)));
        EXPECT_NEAR(q.magnitude(), 1, error);

        Quaternion n = q.normalize();

        EXPECT_NEAR(n.w, q.w, error);
        EXPECT_NEAR(n.v.x, q.v.x, error);
        EXPECT_NEAR(n.v.y, q.v.y, error);
        EXPECT_NEAR(n.v.z, q.v.z, error);
    }
}

TEST(QuaternionTest, Normalise_1) {
    for (int i = 0; i < 10000; i++) {
        Quaternion q = Quaternion::angleAxis(randf(0, 360), Vector3::random(randf(1, 20)));
        Quaternion p = q;

        for (int j = 0; j < 1000; j++) {
            p *= p;
            p = p.normalize();
        }

        EXPECT_NEAR(p.magnitude(), 1, error);
    }
}

TEST(QuaternionTest, Angle0) {
   Quaternion q = Quaternion
}