#include <gtest/gtest.h>
#include <Helpers.h>
#include <Vector3.h>

// rayHitsSphere is the HUD's lock test: does the line the shots travel down
// actually pass through the rock?
class RayHitsSphereTest : public ::testing::Test {
protected:
    const Vector3 origin = Vector3(0, 0, 0);
    const Vector3 forward = Vector3(0, 0, -1); // the ship's own forward axis
};

TEST_F(RayHitsSphereTest, HitsSphereDeadAhead) {
    EXPECT_TRUE(rayHitsSphere(origin, forward, Vector3(0, 0, -50), 6));
}

TEST_F(RayHitsSphereTest, MissesSphereDirectlyBehind) {
    EXPECT_FALSE(rayHitsSphere(origin, forward, Vector3(0, 0, 50), 6));
}

TEST_F(RayHitsSphereTest, MissesSphereOffToTheSide) {
    // 20 units off the ray's line, radius 6: the shots go past it.
    EXPECT_FALSE(rayHitsSphere(origin, forward, Vector3(20, 0, -50), 6));
}

TEST_F(RayHitsSphereTest, HitsSphereOffCentreButWideEnough) {
    // Same offset, but the rock is big enough for the ray to clip its body.
    EXPECT_TRUE(rayHitsSphere(origin, forward, Vector3(20, 0, -50), 21));
}

TEST_F(RayHitsSphereTest, HitsGrazingTheSurface) {
    EXPECT_TRUE(rayHitsSphere(origin, forward, Vector3(5.99, 0, -50), 6));
    EXPECT_FALSE(rayHitsSphere(origin, forward, Vector3(6.01, 0, -50), 6));
}

TEST_F(RayHitsSphereTest, IgnoresASphereBehindEvenWhenItEnclosesTheOrigin) {
    // A centre behind the origin is out, radius regardless - a rock the ship is
    // sitting inside is a collision, not something to draw a lock on.
    EXPECT_FALSE(rayHitsSphere(origin, forward, Vector3(0, 0, 50), 40));
    EXPECT_FALSE(rayHitsSphere(origin, forward, Vector3(0, 0, 50), 60));
}

TEST_F(RayHitsSphereTest, WorksOffTheAxes) {
    Vector3 direction = Vector3(1, 1, 1);
    direction = direction.normalize();

    // Dead on the diagonal: a hit at any radius.
    EXPECT_TRUE(rayHitsSphere(origin, direction, Vector3(30, 30, 30), 1));

    // (40, 30, 20) sits sqrt(200) = 14.1 units off that diagonal.
    EXPECT_FALSE(rayHitsSphere(origin, direction, Vector3(40, 30, 20), 10));
    EXPECT_TRUE(rayHitsSphere(origin, direction, Vector3(40, 30, 20), 15));
}

// formatTime backs the HUD's clock readout.
TEST(FormatTimeTest, FormatsMinutesAndSeconds) {
    EXPECT_EQ(formatTime(0), "0:00");
    EXPECT_EQ(formatTime(5400), "0:05");
    EXPECT_EQ(formatTime(65000), "1:05");
    EXPECT_EQ(formatTime(-100), "0:00");
}
