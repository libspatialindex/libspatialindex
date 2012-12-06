#include <spatialindex/SpatialIndex.h>
#include <spatialindex/GeomUtil.h>

using namespace SpatialIndex;
using namespace GeomUtil;
using namespace std;

/* 
 * Test the GeomUtil
 * Nowhere near complete, but it's something
 */
int main(int argc, char** argv) {
    //define points
    double c1[2] = {1.0, 0.0};
    double c2[2] = {3.0, 2.0};
    double c3[2] = {2.0, 0.0};
    double c4[2] = {2.0, 4.0};
    double c5[2] = {1.0, 1.0};
    double c6[2] = {2.0, 3.0};
    double c7[2] = {1.0, 2.0};
    Point p1 = Point(&c1[0], 2); 
    Point p2 = Point(&c2[0], 2); 
    Point p3 = Point(&c3[0], 2); 
    Point p4 = Point(&c4[0], 2); 
    Point p5 = Point(&c5[0], 2); 
    Point p6 = Point(&c6[0], 2); 
    Point p7 = Point(&c7[0], 2); 
    
    double c3a[2] = {2.0, 3.0};
    Point p3a = Point(&c3a[0], 2); 

    if (!GeomUtil::intersects(p1, p2, p3, p4)) {
        cerr << "Test failed:  intersects returned false, but should be true." << endl;
        cerr << "( " << p1 << ")" << "( " << p2 << ")" << "( " << p3 << ")" << "( " << p4 << ")" << endl;
        return -1;
    }

    if (GeomUtil::intersects(p1, p2, p3a, p4)) {
        cerr << "Test failed:  intersects returned true, but should be false." << endl;
        cerr << "( " << p1 << ")" << "( " << p2 << ")" << "( " << p3a << ")" << "( " << p4 << ")" << endl;
        cerr << GeomUtil::between(p1, p2, p3a) << GeomUtil::between(p1, p2, p4) << GeomUtil::between(p3a, p4, p1) << GeomUtil::between(p3a, p4, p2) << endl; 
        return -1;
    }

    double c3b[2] = {3.0, 2.0};
    Point p3b = Point(&c3b[0], 2); 

    if (GeomUtil::intersectsProper(p1, p2, p3b, p4)) {
        cerr << "Test failed:  intersectsProper returned true, but should be false." << endl;
        cerr << "( " << p1 << ")" << "( " << p2 << ")" << "( " << p3b << ")" << "( " << p4 << ")" << endl;
        return -1;
    }

    if (!GeomUtil::intersects(p1, p2, p3b, p4)) {
        cerr << "Test failed:  intersects returned false, but should be true." << endl;
        cerr << "( " << p1 << ")" << "( " << p2 << ")" << "( " << p3b << ")" << "( " << p4 << ")" << endl;
        return -1;
    }

    //Now Test LineSegment intersection
    LineSegment ls1 = LineSegment(p1, p2);
    LineSegment ls2 = LineSegment(p3, p4);
    LineSegment ls3 = LineSegment(p3a, p4);

    if (!ls1.intersectsShape(ls2)) {
        cerr << "Test failed:  intersectsShape returned false, but should be true." << endl;
        cerr << ls1 << ", " << ls2 << endl;
        return -1;
    }

    if (ls1.intersectsShape(ls3)) {
        cerr << "Test failed:  intersectsShape returned true, but should be false." << endl;
        cerr << ls1 << ", " << ls3 << endl;
        return -1;
    }

    //Now LineSegment Region intersection
    Region r1 = Region(p5, p6);
    Region r2 = Region(p7, p6);
    
    if (!r1.intersectsShape(ls1)) {
        cerr << "Test failed:  intersectsShape returned false, but should be true." << endl;
        cerr << r1 << ", " << ls1 << endl;
        return -1;
    }

    if (r2.intersectsShape(ls1)) {
        cerr << "Test failed:  intersectsShape returned true, but should be false." << endl;
        cerr << r2 << ", " << ls1 << endl;
        return -1;
    }

    return 0;
}
