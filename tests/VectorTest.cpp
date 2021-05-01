#include <Vector3.h>
#include "gtest/gtest.h"
#include <cmath>

TEST(VectorTest, Equality) {
    EXPECT_EQ(Vector3(1,0,0), Vector3(1,0,0));
    EXPECT_EQ(Vector3(0.0556,67,2), Vector3(0.0556,67,2));

    EXPECT_NE(Vector3(4,2,3), Vector3(1,2,3));
    EXPECT_NE(Vector3(1,4,3), Vector3(1,2,3));
    EXPECT_NE(Vector3(1,2,4), Vector3(1,2,3));
}

TEST(VectorTest, DotProduct_Perpendicular) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 1, 0);
    Vector3 c(0, 0, 1);

    EXPECT_EQ(a.dot(b), 0);
    EXPECT_EQ(b.dot(c), 0);
    EXPECT_EQ(c.dot(a), 0);
}

TEST(VectorTest, DotProduct_Commutivity) {
    Vector3 a(2, 3, 1);
    Vector3 b(7, 1, 8);

    EXPECT_DOUBLE_EQ(a.dot(b), b.dot(a));

    Vector3 c(8, -17.6, 0.003);
    Vector3 d(181.65, 2.05, 8.3);

    EXPECT_DOUBLE_EQ(c.dot(d), d.dot(c));
}

TEST(VectorTest, DotProduct_Distributive) {
    Vector3 a(2, 3, 1);
    Vector3 b(7, 1, 8);
    Vector3 c(5, 2, 6);

    EXPECT_DOUBLE_EQ(a.dot(b + c), a.dot(b) + a.dot(c));

    Vector3 d(8, -17.6, 0.003);
    Vector3 e(181.65, 2.05, 8.3);
    Vector3 f(13.78, -56.8, 0.0971);

    EXPECT_DOUBLE_EQ(d.dot(e + f), d.dot(e) + d.dot(f));
}

TEST(VectorTest, DotProduct_Associative) {
    Vector3 a(2, 3, 1);
    Vector3 b(7, 1, 8);

    EXPECT_DOUBLE_EQ((a * 5).dot(b), a.dot(b * 5));

    Vector3 c(8, -17.6, 0.003);
    Vector3 d(181.65, 2.05, 8.3);

    EXPECT_DOUBLE_EQ((c * 5).dot(d), c.dot(d * 5));
}


TEST(VectorTest, CrossProduct) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 1, 0);
    EXPECT_EQ(a.cross(b), Vector3(0, 0, 1));
    EXPECT_EQ(b.cross(a), Vector3(0, 0, -1));
}

TEST(VectorTest, CrossProduct_NotAssociative) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 1, 0);
    EXPECT_NE(a.cross(b), b.cross(a));
}

TEST(VectorTest, AngleBettween_90) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 1, 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), M_PI/2);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), M_PI/2);
}

TEST(VectorTest, AngleBettween_60) {
    Vector3 a(1, 0, 0);
    Vector3 b(1, sqrt(3), 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), M_PI/3);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), M_PI/3);
}

TEST(VectorTest, AngleBettween_30) {
    Vector3 a(1, 0, 0);
    Vector3 b(sqrt(3), 1, 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), M_PI/6);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), M_PI/6);
}

TEST(VectorTest, AngleBettween_45) {
    Vector3 a(1, 0, 0);
    Vector3 b(1, 1, 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), M_PI/4);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), M_PI/4);
}


TEST(VectorTest, AngleBettween_0) {
    Vector3 a(1, 0, 0);
    Vector3 b(1, 0, 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), 0);
}

TEST(VectorTest, AngleBettween_ZeroVector) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 0, 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(a, b), 0);
    EXPECT_DOUBLE_EQ(Vector3::angle(b, a), 0);
}
