/*
 * Codex wrote this gtest port of the original geometry intersection test.
 * The old standalone test program has been converted into individual
 * GoogleTest cases that exercise LineSegment and Region intersection logic
 * directly through the libspatialindex C++ API.
 */

#include <spatialindex/SpatialIndex.h>

#include "test.h"

TEST(GeometryRegionTest, DisjointRegionsDoNotTouch) {
    double low[2] = {0.0, 0.0};
    double high[2] = {2.0, 2.0};
    SpatialIndex::Region region(low, high, 2);
    // Each case shares a low or high coordinate, but has a strict gap on
    // the other axis. The positive gap independently rules out contact.
    const double bounds[][4] = {
        {0, 3, 1, 4}, {1, 3, 2, 4}, {3, 0, 4, 1}, {3, 1, 4, 2},
        {0, -2, 1, -1}, {1, -2, 2, -1}, {-2, 0, -1, 1}, {-2, 1, -1, 2}
    };
    for (const auto& coordinates : bounds) {
        double otherLow[2] = {coordinates[0], coordinates[1]};
        double otherHigh[2] = {coordinates[2], coordinates[3]};
        SpatialIndex::Region other(otherLow, otherHigh, 2);
        ASSERT_TRUE(otherLow[0] > high[0] || otherHigh[0] < low[0] ||
                    otherLow[1] > high[1] || otherHigh[1] < low[1]);
        ASSERT_GT(region.getMinimumDistance(other), 0.0);
        EXPECT_FALSE(region.touchesRegion(other));
        EXPECT_FALSE(other.touchesRegion(region));
        EXPECT_FALSE(region.touchesShape(other));
    }
}

TEST(GeometryRegionTest, OutsidePointsDoNotTouch) {
    double low[2] = {0.0, 0.0};
    double high[2] = {2.0, 2.0};
    SpatialIndex::Region region(low, high, 2);
    const double coordinates[][2] = {
        {0, 3}, {2, 3}, {0, -1}, {2, -1},
        {3, 0}, {3, 2}, {-1, 0}, {-1, 2}
    };
    for (const auto& coordinate : coordinates) {
        SpatialIndex::Point point(coordinate, 2);
        ASSERT_TRUE(coordinate[0] < low[0] || coordinate[0] > high[0] ||
                    coordinate[1] < low[1] || coordinate[1] > high[1]);
        ASSERT_GT(region.getMinimumDistance(point), 0.0);
        EXPECT_FALSE(region.touchesPoint(point));
        EXPECT_FALSE(region.touchesShape(point));
    }
}

TEST(GeometryRegionTest, BoundaryPointsStillTouch) {
    double low[2] = {0.0, 0.0};
    double high[2] = {2.0, 2.0};
    SpatialIndex::Region region(low, high, 2);
    const double coordinates[][2] = {
        {0, 0}, {0, 2}, {2, 0}, {2, 2}, {0, 1}, {1, 0}, {2, 1}, {1, 2}
    };
    for (const auto& coordinate : coordinates) {
        SpatialIndex::Point point(coordinate, 2);
        EXPECT_TRUE(region.touchesPoint(point));
    }
}

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
