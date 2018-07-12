#include "gmock/gmock.h"
#include "ik/ik.h"

#define NAME mat3x3

using namespace ::testing;

TEST(NAME, from_quat_1)
{
    ik_mat3x3_t m;
    ik_quat_t q;
    IKAPI.quat.set_identity(q.f);
    IKAPI.mat3x3.from_quat(m.f, q.f);

    ikreal_t ex[3] = {1, 0, 0};
    ikreal_t ey[3] = {0, 1, 0};
    ikreal_t ez[3] = {0, 0, 1};
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ex[i], DoubleEq(ex[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ey[i], DoubleEq(ey[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ez[i], DoubleEq(ez[i]));
}

TEST(NAME, from_quat_2)
{
    ik_mat3x3_t m;
    ik_quat_t q;
    ik_vec3_t axis = IKAPI.vec3.vec3(0, 1, 0);
    IKAPI.quat.set_axis_angle(q.f, axis.f, 45.0*M_PI/180);
    IKAPI.mat3x3.from_quat(m.f, q.f);

    ikreal_t ex[3] = {1/sqrt(2), 0, 1/sqrt(2)};
    ikreal_t ey[3] = {0, 1, 0};
    ikreal_t ez[3] = {-1/sqrt(2), 0, 1/sqrt(2)};
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ex[i], DoubleEq(ex[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ey[i], DoubleEq(ey[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ez[i], DoubleEq(ez[i]));
}

TEST(NAME, from_quat_3)
{
    ik_mat3x3_t m;
    ik_quat_t q;
    ik_vec3_t axis = IKAPI.vec3.vec3(1, 0, 0);
    IKAPI.quat.set_axis_angle(q.f, axis.f, 45.0*M_PI/180);
    IKAPI.mat3x3.from_quat(m.f, q.f);

    ikreal_t ex[3] = {1, 0, 0};
    ikreal_t ey[3] = {0, 1/sqrt(2), -1/sqrt(2)};
    ikreal_t ez[3] = {0, 1/sqrt(2), 1/sqrt(2)};
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ex[i], DoubleEq(ex[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ey[i], DoubleEq(ey[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ez[i], DoubleEq(ez[i]));
}

TEST(NAME, from_quat_4)
{
    ik_mat3x3_t m;
    ik_quat_t q;
    ik_vec3_t axis = IKAPI.vec3.vec3(0, 0, 1);
    IKAPI.quat.set_axis_angle(q.f, axis.f, 45.0*M_PI/180);
    IKAPI.mat3x3.from_quat(m.f, q.f);

    ikreal_t ex[3] = {1/sqrt(2), -1/sqrt(2), 0};
    ikreal_t ey[3] = {1/sqrt(2), 1/sqrt(2), 0};
    ikreal_t ez[3] = {0, 0, 1};
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ex[i], DoubleEq(ex[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ey[i], DoubleEq(ey[i]));
    for (int i = 0; i != 3; ++i)
        EXPECT_THAT(m.ez[i], DoubleEq(ez[i]));
}