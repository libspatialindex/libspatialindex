/*
 * Codex wrote this gtest port of the original geometry intersection test.
 * The old standalone test program has been converted into individual
 * GoogleTest cases that exercise LineSegment and Region intersection logic
 * directly through the libspatialindex C++ API.
 */

#include <spatialindex/SpatialIndex.h>

#include "test.h"

TEST(GeometryLineSegmentTest, IntersectsLineSegments) {
    double c1[2] = {1.0, 0.0};
    double c2[2] = {3.0, 2.0};
    double c3[2] = {2.0, 0.0};
    double c4[2] = {2.0, 4.0};
    double c3a[2] = {2.0, 3.0};

    SpatialIndex::Point p1(c1, 2);
    SpatialIndex::Point p2(c2, 2);
    SpatialIndex::Point p3(c3, 2);
    SpatialIndex::Point p4(c4, 2);
    SpatialIndex::Point p3a(c3a, 2);

    SpatialIndex::LineSegment ls1(p1, p2);
    SpatialIndex::LineSegment ls2(p3, p4);
    SpatialIndex::LineSegment ls3(p3a, p4);

    EXPECT_TRUE(ls1.intersectsShape(ls2));
    EXPECT_FALSE(ls1.intersectsShape(ls3));
}

TEST(GeometryLineSegmentTest, IntersectsRegions) {
    double c1[2] = {1.0, 0.0};
    double c2[2] = {3.0, 2.0};
    double c5[2] = {1.0, 1.0};
    double c6[2] = {2.5, 3.0};
    double c7[2] = {1.0, 2.0};
    double c8[2] = {0.0, -1.0};
    double c9[2] = {4.0, 3.0};

    SpatialIndex::Point p1(c1, 2);
    SpatialIndex::Point p2(c2, 2);
    SpatialIndex::Point p5(c5, 2);
    SpatialIndex::Point p6(c6, 2);
    SpatialIndex::Point p7(c7, 2);
    SpatialIndex::Point p8(c8, 2);
    SpatialIndex::Point p9(c9, 2);

    SpatialIndex::LineSegment ls1(p1, p2);
    SpatialIndex::Region r1(p5, p6);
    SpatialIndex::Region r2(p7, p6);
    SpatialIndex::Region r3(p8, p9);

    EXPECT_TRUE(r1.intersectsShape(ls1));
    EXPECT_TRUE(ls1.intersectsShape(r1));

    EXPECT_FALSE(r2.intersectsShape(ls1));
    EXPECT_FALSE(ls1.intersectsShape(r2));

    EXPECT_TRUE(r3.intersectsShape(ls1));
    EXPECT_TRUE(ls1.intersectsShape(r3));
}

TEST(GeometryLineSegmentTest, MinimumDistanceToPoint) {
    double s[2] = {0.0, 0.0};
    double e[2] = {4.0, 0.0};
    SpatialIndex::LineSegment ls(s, e, 2);

    // closest to the interior of the segment
    double c1[2] = {2.0, 3.0};
    EXPECT_DOUBLE_EQ(ls.getMinimumDistance(SpatialIndex::Point(c1, 2)), 3.0);

    // collinear with the segment but past its end point
    double c2[2] = {7.0, 0.0};
    EXPECT_DOUBLE_EQ(ls.getMinimumDistance(SpatialIndex::Point(c2, 2)), 3.0);

    // beyond the start point, off the line
    double c3[2] = {-3.0, 4.0};
    EXPECT_DOUBLE_EQ(ls.getMinimumDistance(SpatialIndex::Point(c3, 2)), 5.0);

    // diagonal segment, point beyond the end point
    double ds[2] = {0.0, 0.0};
    double de[2] = {1.0, 1.0};
    SpatialIndex::LineSegment diag(ds, de, 2);
    double c4[2] = {4.0, 5.0};
    EXPECT_DOUBLE_EQ(diag.getMinimumDistance(SpatialIndex::Point(c4, 2)), 5.0);

    // a Ball far away from the segment does not intersect it
    double bc[2] = {10.0, 0.0};
    SpatialIndex::Ball ball(1.0, SpatialIndex::Point(bc, 2));
    EXPECT_FALSE(ball.intersectsShape(ls));
    EXPECT_DOUBLE_EQ(ball.getMinimumDistance(ls), 5.0);
}
