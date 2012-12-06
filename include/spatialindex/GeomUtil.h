#pragma once

#include <spatialindex/SpatialIndex.h>

/* Namespace for generic geometry functions */
namespace GeomUtil {

    /*
     * ******
     * All functions below assume
     * 2 dimensions (index 0, 1 for X, Y) and
     * counter-clockwise orientation of points.
     * ******
     */

    /* 
     * Calculate 2X the area of the triangle consisting of 
     * points a, b, and c.  If c is Right of segment (a, b), 
     * then area2 will be negative.
     */
    double area2(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c);

    // is point c to the "left" of the segment defined by points a, b
    bool left(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c);

    // Are a, b, and c on the same line
    bool collinear(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c);

    // Do the segments (a, b), (c, d) intersect "Properly"
    // (i.e. endpoints not included)
    bool intersectsProper(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c, const SpatialIndex::Point &d);

    // If a, b and c are collinear, is c between a and b
    bool between(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c);

    // is c between a and b 
    bool between(double a, double b, double c);

    // Do the segments (a, b), (c, d) intersect at all
    // (endpoints are included)
    bool intersects(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c, const SpatialIndex::Point &d);

}
