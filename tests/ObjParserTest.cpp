#include <Helpers.h>
#include "gtest/gtest.h"
#include "../src/Quaternion.h"

TEST(QuaternionTest, InitialisesToIdentityQuaternion) {
    Quaternion q;
    EXPECT_EQ(q.v, Vector3(0, 0, 0));
    EXPECT_EQ(q.w, 1);
}
