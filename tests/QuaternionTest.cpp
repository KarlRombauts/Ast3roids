#include "gtest/gtest.h"
#include "../src/Quaternion.h"

double error = 0.000000001;

TEST(QuaternionTest, InitialisesToIdentityQuaternion) {
    Quaternion q;
    EXPECT_EQ(q.x, 0);
    EXPECT_EQ(q.y, 0);
    EXPECT_EQ(q.z, 0);
    EXPECT_EQ(q.w, 1);
}

TEST(QuaternionTest, MultiplicationOfIdentityReturnsIdentity) {
    Quaternion a;
    Quaternion b;
    Quaternion q = a * b;

    EXPECT_NEAR(q.x, 0, error);
    EXPECT_NEAR(q.y, 0, error);
    EXPECT_NEAR(q.z, 0, error);
    EXPECT_NEAR(q.w, 1, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_NoRotation) {
    Quaternion q = Quaternion::angleAxis(0, Vector3::up());

    EXPECT_NEAR(q.x, 0, error);
    EXPECT_NEAR(q.y, 0, error);
    EXPECT_NEAR(q.z, 0, error);
    EXPECT_NEAR(q.w, 1, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_90_Degrees) {
    Quaternion q = Quaternion::angleAxis(90, Vector3::up());

    EXPECT_NEAR(q.x, 0, error);
    EXPECT_NEAR(q.y, 1/sqrt(2), error);
    EXPECT_NEAR(q.z, 0, error);
    EXPECT_NEAR(q.w, 1/sqrt(2), error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_180_Degrees) {
    Quaternion q = Quaternion::angleAxis(180, Vector3::up());
    EXPECT_NEAR(q.x, 0, error);
    EXPECT_NEAR(q.y, 1, error);
    EXPECT_NEAR(q.z, 0, error);
    EXPECT_NEAR(q.w, 0, error);
}

TEST(QuaternionTest, IntialiseAngleAxis_Up_270_Degrees) {
    Quaternion q = Quaternion::angleAxis(270, Vector3::up());
    EXPECT_NEAR(q.x, 0, error);
    EXPECT_NEAR(q.y, 1/sqrt(2), error);
    EXPECT_NEAR(q.z, 0, error);
    EXPECT_NEAR(q.w, -1/sqrt(2), error);
}
