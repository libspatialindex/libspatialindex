#include <iostream>
#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;

int main(int argc, char** argv) {
    double c1[2] = {1.0, 0.0};
    Point p1 = Point(&c1[0], 2);
    std::cout << "done" << std::endl;
    return(0);
}
