#include <Vector3.h>
#include "gtest/gtest.h"
#include <cmath>
#include <Helpers.h>

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

TEST(VectorTest, CrossProduct_2) {
    Vector3 a(5, 1, 2);
    Vector3 b(1, 7, -3);
    EXPECT_EQ(a.cross(b), Vector3(-17, 17, 34));
    EXPECT_EQ(b.cross(a), Vector3(17, -17, -34));
}

TEST(VectorTest, CrossProduct_3) {
    Vector3 a(0.56, 8.97, 2.05);
    Vector3 b(-68.19, -0.79, 5832.13);

    const Vector3 &cross = a.cross(b);

    EXPECT_DOUBLE_EQ(cross.x, 52315.8256);
    EXPECT_DOUBLE_EQ(cross.y, -3405.7823);
    EXPECT_DOUBLE_EQ(cross.z, 611.2219);
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

TEST(VectorTest, Colinearity) {
    Vector3 a(1, 0, 0);
    Vector3 b(1, 0, 0);
    EXPECT_TRUE(a.isCollinear(b));
}

TEST(VectorTest, Colinearity_2) {
    Vector3 a(2, 4, 8);
    Vector3 b(1, 2, 4);
    EXPECT_TRUE(a.isCollinear(b));
}

TEST(VectorTest, Colinearity_3) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 0, 0);
    EXPECT_TRUE(a.isCollinear(b));
}

TEST(VectorTest, LinearIndependant) {
    Vector3 a(1, 0, 0);
    Vector3 b(0, 1, 0);
    EXPECT_FALSE(a.isCollinear(b));
}

TEST(VectorTest, Orthoganalise_2) {
    Vector3 a(0, 0, 1);
    Vector3 b(0, 1, 1);

    const Vector3 &o = b.orthogonalize(a);

    EXPECT_EQ(o.x, 0);
    EXPECT_EQ(o.y, 1);
    EXPECT_EQ(o.z, 0);
}

TEST(VectorTest, Orthoganalise_3) {
    Vector3 a(sqrt(2), 1, 0);
    Vector3 b(1, sqrt(2), 0);

    const Vector3 &o = b.orthogonalize(a);

    Vector3 expected = Vector3(-1, sqrt(2), 0).normalize();
    EXPECT_DOUBLE_EQ(o.x, expected.x);
    EXPECT_DOUBLE_EQ(o.y, expected.y);
    EXPECT_DOUBLE_EQ(o.z, expected.z);
}

TEST(VectorTest, Orthoganalise_4) {
    Vector3 a(sqrt(2), 1, 0);
    Vector3 b(-sqrt(2), 1, 0);

    const Vector3 &o = b.orthogonalize(a);

    Vector3 expected = Vector3(-1, sqrt(2), 0).normalize();

    EXPECT_DOUBLE_EQ(o.x, expected.x);
    EXPECT_DOUBLE_EQ(o.y, expected.y);
    EXPECT_DOUBLE_EQ(o.z, expected.z);
}

TEST(VectorTest, Orthoganalise_5) {
    Vector3 a(sqrt(2), 1, 0);
    Vector3 b(0, -1, 0);

    const Vector3 &o = b.orthogonalize(a);

    Vector3 expected = Vector3(1, -sqrt(2), 0).normalize();

    EXPECT_DOUBLE_EQ(o.x, expected.x);
    EXPECT_DOUBLE_EQ(o.y, expected.y);
    EXPECT_DOUBLE_EQ(o.z, expected.z);
}

TEST(VectorTest, Orthoganalise_Dot) {
    for(int i = 0; i < 100000; i++) {
        Vector3 a(randf(-100, 100), randf(-100, 100), randf(-100, 100));
        Vector3 b(randf(-100, 100), randf(-100, 100), randf(-100, 100));
        Vector3 o = b.orthogonalize(a);
        EXPECT_NEAR(o.dot(a), 0, 0.00000001);
    }
}
