// Copy, assignment and re-dimensioning of TimeRegion and TimePoint.
//
// Region and Point keep up to three dimensions in an inline buffer and
// larger ones in a single heap block (#255). These tests cover both paths,
// and changes between them, so that AddressSanitizer catches leaks and
// mismatched frees in the derived time shapes.

#include <spatialindex/SpatialIndex.h>

#include "test.h"

#include <vector>

using namespace SpatialIndex;

namespace {

TimeRegion makeTimeRegion(uint32_t dim, double offset)
{
    std::vector<double> low(dim), high(dim);
    for (uint32_t i = 0; i < dim; ++i)
    {
        low[i] = offset + i;
        high[i] = offset + i + 1;
    }
    return TimeRegion(low.data(), high.data(), 1.0 + offset, 2.0 + offset, dim);
}

TimePoint makeTimePoint(uint32_t dim, double offset)
{
    std::vector<double> coords(dim);
    for (uint32_t i = 0; i < dim; ++i)
        coords[i] = offset + i;
    return TimePoint(coords.data(), 1.0 + offset, 2.0 + offset, dim);
}

} // namespace

class TimeShapeDimensionTest : public testing::TestWithParam<uint32_t> {};

TEST_P(TimeShapeDimensionTest, TimeRegionCopyAndAssign) {
    const uint32_t dim = GetParam();
    TimeRegion a = makeTimeRegion(dim, 0);

    TimeRegion copy(a);
    EXPECT_TRUE(copy == a);

    std::unique_ptr<TimeRegion> clone(a.clone());
    EXPECT_TRUE(*clone == a);

    TimeRegion assigned;
    assigned = a;
    EXPECT_TRUE(assigned == a);

    // Assign across the inline / heap boundary in both directions.
    for (uint32_t other : {2u, 5u})
    {
        TimeRegion b = makeTimeRegion(other, 10);
        b = a;
        EXPECT_TRUE(b == a);
        EXPECT_EQ(dim, b.getDimension());
        EXPECT_DOUBLE_EQ(a.getLow(dim - 1), b.getLow(dim - 1));
        EXPECT_DOUBLE_EQ(a.getHigh(dim - 1), b.getHigh(dim - 1));
    }

    TimeRegion infinite = makeTimeRegion(dim == 2 ? 5 : 2, 0);
    infinite.makeInfinite(dim);
    EXPECT_EQ(dim, infinite.getDimension());
}

TEST_P(TimeShapeDimensionTest, TimePointCopyAndAssign) {
    const uint32_t dim = GetParam();
    TimePoint a = makeTimePoint(dim, 0);

    TimePoint copy(a);
    EXPECT_TRUE(copy == a);

    std::unique_ptr<TimePoint> clone(a.clone());
    EXPECT_TRUE(*clone == a);

    TimePoint assigned;
    assigned = a;
    EXPECT_TRUE(assigned == a);

    for (uint32_t other : {2u, 5u})
    {
        TimePoint b = makeTimePoint(other, 10);
        b = a;
        EXPECT_TRUE(b == a);
        EXPECT_EQ(dim, b.getDimension());
        EXPECT_DOUBLE_EQ(a.getCoordinate(dim - 1), b.getCoordinate(dim - 1));
    }

    TimePoint infinite = makeTimePoint(dim == 2 ? 5 : 2, 0);
    infinite.makeInfinite(dim);
    EXPECT_EQ(dim, infinite.getDimension());
}

// 2 fits the inline buffer, 5 needs the heap.
INSTANTIATE_TEST_SUITE_P(InlineAndHeap, TimeShapeDimensionTest, testing::Values(2u, 5u));
