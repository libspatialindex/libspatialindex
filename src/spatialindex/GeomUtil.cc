#include <spatialindex/GeomUtil.h>

double GeomUtil::area2(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c) {
    double *pA, *pB, *pC;
    pA = a.m_pCoords; pB = b.m_pCoords; pC = c.m_pCoords;
    return (((pB[0] - pA[0]) * (pC[1] - pA[1])) - ((pC[0] - pA[0]) * (pB[1] - pA[1])));
}

bool GeomUtil::left(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c) {
    return (area2(a, b, c) > 0);
}

bool GeomUtil::collinear(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c) {
    return (area2(a, b, c) == 0);
}

bool GeomUtil::intersectsProper(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c, const SpatialIndex::Point &d) {
    if ( collinear(a, b, c) || collinear(a, b, d) ||
         collinear(c, d, a) || collinear(c, d, b)) {
        return false;
    }
    return ((left(a, b, c) ^ left(a, b, d)) &&
            (left(c, d, a) ^ left(c, d, b)));
}

bool GeomUtil::between(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c) {
    if ( !collinear(a, b, c) ) {
        return false;
    }
    double *pA, *pB, *pC;
    pA = a.m_pCoords; pB = b.m_pCoords; pC = c.m_pCoords;
    if ( pA[0] != pB[0] ) { // a & b are not on the same vertical, compare on x axis
        return  between(pA[0], pB[0], pC[0]);
    } else { // a & b are a vertical segment, we need to compare on y axis
        return between(pA[1], pB[1], pC[1]);
    }
}

bool GeomUtil::between(double a, double b, double c) {
    return ( ((a <= c) && (c <= b)) || ((a >= c) && (c >= b)) );
}

bool GeomUtil::intersects(const SpatialIndex::Point &a, const SpatialIndex::Point &b, const SpatialIndex::Point &c, const SpatialIndex::Point &d) {
    if (intersectsProper(a, b, c, d)) {
        return true;
    } 
    else if ( between(a, b, c) || between(a, b, d) ||
              between(c, d, a) || between(c, d, b) ) { 
        return true;
    }
    else { 
        return false;
    }
}
