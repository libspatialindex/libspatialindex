// Construction, copy, assignment, serialization and re-dimensioning of
// MovingRegion and MovingPoint.
//
// Region and Point keep up to three dimensions in an inline buffer and larger
// ones on the heap (#255). These tests cover both paths, and changes between
// them, so that AddressSanitizer catches leaks and mismatched frees in the
// TPR-tree shapes.

#include <spatialindex/SpatialIndex.h>

#include "test.h"

#include <memory>
#include <vector>

using namespace SpatialIndex;

namespace {

MovingRegion makeMovingRegion(uint32_t dim, double offset)
{
    std::vector<double> low(dim), high(dim), vlow(dim), vhigh(dim);
    for (uint32_t i = 0; i < dim; ++i)
    {
        low[i] = offset + i;
        high[i] = offset + i + 1;
        vlow[i] = -0.5 - i;
        vhigh[i] = 0.5 + i;
    }
    return MovingRegion(low.data(), high.data(), vlow.data(), vhigh.data(), 0.0, 10.0 + offset, dim);
}

MovingPoint makeMovingPoint(uint32_t dim, double offset)
{
    std::vector<double> coords(dim), vcoords(dim);
    for (uint32_t i = 0; i < dim; ++i)
    {
        coords[i] = offset + i;
        vcoords[i] = 0.25 + i;
    }
    return MovingPoint(coords.data(), vcoords.data(), 0.0, 10.0 + offset, dim);
}

void expectSameRegion(const MovingRegion& a, const MovingRegion& b)
{
    ASSERT_EQ(a.getDimension(), b.getDimension());
    for (uint32_t i = 0; i < a.getDimension(); ++i)
    {
        EXPECT_DOUBLE_EQ(a.m_pLow[i], b.m_pLow[i]);
        EXPECT_DOUBLE_EQ(a.m_pHigh[i], b.m_pHigh[i]);
        EXPECT_DOUBLE_EQ(a.m_pVLow[i], b.m_pVLow[i]);
        EXPECT_DOUBLE_EQ(a.m_pVHigh[i], b.m_pVHigh[i]);
    }
    EXPECT_DOUBLE_EQ(a.getLowerBound(), b.getLowerBound());
    EXPECT_DOUBLE_EQ(a.getUpperBound(), b.getUpperBound());
}

void expectSamePoint(const MovingPoint& a, const MovingPoint& b)
{
    ASSERT_EQ(a.getDimension(), b.getDimension());
    for (uint32_t i = 0; i < a.getDimension(); ++i)
    {
        EXPECT_DOUBLE_EQ(a.m_pCoords[i], b.m_pCoords[i]);
        EXPECT_DOUBLE_EQ(a.m_pVCoords[i], b.m_pVCoords[i]);
    }
    EXPECT_DOUBLE_EQ(a.getLowerBound(), b.getLowerBound());
    EXPECT_DOUBLE_EQ(a.getUpperBound(), b.getUpperBound());
}

} // namespace

class MovingShapeDimensionTest : public testing::TestWithParam<uint32_t> {};

TEST_P(MovingShapeDimensionTest, MovingRegionCopyAssignAndSerialize) {
    const uint32_t dim = GetParam();
    MovingRegion a = makeMovingRegion(dim, 0);

    MovingRegion copy(a);
    expectSameRegion(a, copy);

    std::unique_ptr<MovingRegion> clone(a.clone());
    expectSameRegion(a, *clone);

    MovingRegion assigned;
    assigned = a;
    expectSameRegion(a, assigned);

    // Assign across the inline / heap boundary in both directions.
    for (uint32_t other : {2u, 5u})
    {
        MovingRegion b = makeMovingRegion(other, 10);
        b = a;
        expectSameRegion(a, b);
    }

    uint8_t* data = nullptr;
    uint32_t length = 0;
    a.storeToByteArray(&data, length);
    MovingRegion loaded = makeMovingRegion(dim == 2 ? 5 : 2, 20);
    loaded.loadFromByteArray(data);
    delete[] data;
    expectSameRegion(a, loaded);

    MovingRegion infinite = makeMovingRegion(dim == 2 ? 5 : 2, 0);
    infinite.makeInfinite(dim);
    EXPECT_EQ(dim, infinite.getDimension());

    // Built from two moving points.
    MovingPoint low = makeMovingPoint(dim, 0), high = makeMovingPoint(dim, 1);
    MovingRegion fromPoints(low, high);
    EXPECT_EQ(dim, fromPoints.getDimension());
    EXPECT_DOUBLE_EQ(high.m_pCoords[dim - 1], fromPoints.m_pHigh[dim - 1]);
    EXPECT_DOUBLE_EQ(low.m_pVCoords[dim - 1], fromPoints.m_pVLow[dim - 1]);
}

TEST_P(MovingShapeDimensionTest, MovingPointCopyAssignAndSerialize) {
    const uint32_t dim = GetParam();
    MovingPoint a = makeMovingPoint(dim, 0);

    MovingPoint copy(a);
    expectSamePoint(a, copy);

    std::unique_ptr<MovingPoint> clone(a.clone());
    expectSamePoint(a, *clone);

    MovingPoint assigned;
    assigned = a;
    expectSamePoint(a, assigned);

    for (uint32_t other : {2u, 5u})
    {
        MovingPoint b = makeMovingPoint(other, 10);
        b = a;
        expectSamePoint(a, b);
    }

    uint8_t* data = nullptr;
    uint32_t length = 0;
    a.storeToByteArray(&data, length);
    MovingPoint loaded = makeMovingPoint(dim == 2 ? 5 : 2, 20);
    loaded.loadFromByteArray(data);
    delete[] data;
    expectSamePoint(a, loaded);

    MovingPoint infinite = makeMovingPoint(dim == 2 ? 5 : 2, 0);
    infinite.makeInfinite(dim);
    EXPECT_EQ(dim, infinite.getDimension());
}

// 2 fits the inline buffer, 5 needs the heap.
INSTANTIATE_TEST_SUITE_P(InlineAndHeap, MovingShapeDimensionTest, testing::Values(2u, 5u));

TEST(MovingShapeTest, DefaultConstructedShapesCanBeDestroyed) {
    // MovingPoint's velocity pointer used to be left uninitialized by the
    // default constructor and then deleted by the destructor.
    { MovingPoint p; }
    { MovingRegion r; }
    SUCCEED();
}
