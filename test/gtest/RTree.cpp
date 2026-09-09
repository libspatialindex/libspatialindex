/*
 * Codex wrote this gtest port of the legacy RTree shell-script tests.
 * The script-driven generator/load/query/self-join checks were replaced
 * with native GoogleTest cases that build in-memory RTree instances, perform
 * the same classes of operations, and compare the tree results with local
 * exhaustive expectations.
 */

#include "NativeTestSupport.h"

TEST(RTreeTest, NearestNeighborQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    // Keep an independent copy of the indexed rectangles so the expected
    // nearest-neighbor set is computed without asking the RTree.
    std::map<SpatialIndex::id_type, sidx_test::Rect> active;
    for (SpatialIndex::id_type id = 1; id <= 12; ++id) {
        double x = static_cast<double>(id % 4) * 0.25;
        double y = static_cast<double>(id / 4) * 0.25;
        sidx_test::Rect rect(x, y, x + 0.04, y + 0.04);
        sidx_test::insertRect(*tree, id, rect);
        active[id] = rect;
    }

    sidx_test::Rect query(0.31, 0.22, 0.31, 0.22);
    sidx_test::IdVisitor visitor;
    SpatialIndex::Point point = sidx_test::pointFromRectLow(query);
    tree->nearestNeighborQuery(10, point, visitor);

    sidx_test::expectSameIds(sidx_test::nearestIds(active, query, 10), visitor.ids);
    EXPECT_TRUE(tree->isIndexValid());
}

TEST(RTreeTest, MixedIntersectionQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    // This mirrors the old mixed command stream: load a few rectangles,
    // query, delete one, insert another, and query the changed contents.
    std::map<SpatialIndex::id_type, sidx_test::Rect> active;

    sidx_test::Rect r1(0.0, 0.0, 0.3, 0.3);
    sidx_test::Rect r2(0.2, 0.2, 0.5, 0.5);
    sidx_test::Rect r3(0.6, 0.6, 0.9, 0.9);
    sidx_test::Rect r4(0.4, 0.1, 0.7, 0.4);

    sidx_test::insertRect(*tree, 1, r1);
    active[1] = r1;
    sidx_test::insertRect(*tree, 2, r2);
    active[2] = r2;
    sidx_test::insertRect(*tree, 3, r3);
    active[3] = r3;

    sidx_test::Rect firstQuery(0.25, 0.25, 0.65, 0.65);
    SpatialIndex::Region firstRegion = sidx_test::regionFromRect(firstQuery);
    sidx_test::expectSameIds(
        sidx_test::intersectingIds(active, firstQuery),
        sidx_test::queryIds(*tree, firstRegion));

    ASSERT_TRUE(sidx_test::deleteRect(*tree, 2, r2));
    active.erase(2);

    sidx_test::insertRect(*tree, 4, r4);
    active[4] = r4;

    sidx_test::Rect secondQuery(0.35, 0.05, 0.75, 0.45);
    SpatialIndex::Region secondRegion = sidx_test::regionFromRect(secondQuery);
    sidx_test::expectSameIds(
        sidx_test::intersectingIds(active, secondQuery),
        sidx_test::queryIds(*tree, secondRegion));

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(RTreeTest, BulkLoadedIntersectionQueriesMatchExhaustiveSearch) {
    std::vector<std::pair<SpatialIndex::id_type, sidx_test::Rect> > entries;
    std::map<SpatialIndex::id_type, sidx_test::Rect> active;

    // Feed the bulk loader with deterministic grid data, then use the same
    // grid as the brute-force oracle for intersection queries.
    for (SpatialIndex::id_type id = 0; id < 30; ++id) {
        double x = static_cast<double>(id % 6) * 0.16;
        double y = static_cast<double>(id / 6) * 0.16;
        sidx_test::Rect rect(x, y, x + 0.08, y + 0.08);
        entries.push_back(std::make_pair(id, rect));
        active[id] = rect;
    }

    sidx_test::RTreeDataStream stream(entries);
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createAndBulkLoadNewRTree(
            SpatialIndex::RTree::BLM_STR,
            stream,
            *storage,
            0.9,
            20,
            20,
            2,
            SpatialIndex::RTree::RV_RSTAR,
            indexIdentifier));

    sidx_test::Rect query(0.2, 0.2, 0.55, 0.55);
    SpatialIndex::Region region = sidx_test::regionFromRect(query);
    sidx_test::expectSameIds(
        sidx_test::intersectingIds(active, query),
        sidx_test::queryIds(*tree, region));

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(RTreeTest, SelfJoinQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    // Self-join reports every intersecting pair inside the query window; the
    // helper computes the same directed pairs by scanning all rectangles.
    std::map<SpatialIndex::id_type, sidx_test::Rect> active;
    active[1] = sidx_test::Rect(0.0, 0.0, 0.4, 0.4);
    active[2] = sidx_test::Rect(0.2, 0.2, 0.5, 0.5);
    active[3] = sidx_test::Rect(0.6, 0.6, 0.9, 0.9);
    active[4] = sidx_test::Rect(0.35, 0.1, 0.7, 0.3);

    for (std::map<SpatialIndex::id_type, sidx_test::Rect>::const_iterator it = active.begin();
         it != active.end();
         ++it) {
        sidx_test::insertRect(*tree, it->first, it->second);
    }

    sidx_test::Rect query(0.0, 0.0, 0.75, 0.55);
    SpatialIndex::Region region = sidx_test::regionFromRect(query);
    sidx_test::IdVisitor visitor;
    tree->selfJoinQuery(region, visitor);

    sidx_test::expectSamePairs(sidx_test::selfJoinPairs(active, query), visitor.pairs);
    EXPECT_TRUE(tree->isIndexValid());
}

// Regression test for https://github.com/libspatialindex/libspatialindex/issues/107
//
// A NaN (or Infinite) coordinate in an inserted region used to be accepted
// silently. Because comparisons against NaN are always false, it poisons
// every margin/area comparison it participates in, which can leave
// Node::rstarSplit()'s chooseSplitAxis() step unable to select a split axis
// (splitAxis stays at its uint32_t sentinel). That sentinel is then used to
// index Region::m_pLow/m_pHigh in RstarSplitEntry::compareHigh(), causing an
// out-of-bounds read/segfault -- often several inserts after the actual bad
// data, on a split that looks unrelated. insertData() now rejects non-finite
// coordinates immediately with a clear exception instead.
TEST(RTreeTest, InsertingNonFiniteCoordinateThrowsInsteadOfCorruptingTree) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 4, 4, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    double nan_low[2] = { std::numeric_limits<double>::quiet_NaN(), 1.0 };
    double nan_high[2] = { 2.0, 2.0 };
    SpatialIndex::Region nanRegion(nan_low, nan_high, 2);

    EXPECT_THROW(
        tree->insertData(0, nullptr, nanRegion, 1),
        Tools::IllegalArgumentException);

    double inf_low[2] = { 1.0, 1.0 };
    double inf_high[2] = { std::numeric_limits<double>::infinity(), 2.0 };
    SpatialIndex::Region infRegion(inf_low, inf_high, 2);

    EXPECT_THROW(
        tree->insertData(0, nullptr, infRegion, 2),
        Tools::IllegalArgumentException);

    // The tree must still be perfectly usable afterwards -- rejecting bad
    // input should not leave any partial state behind.
    sidx_test::Rect ok(0.0, 0.0, 1.0, 1.0);
    sidx_test::insertRect(*tree, 3, ok);
    EXPECT_TRUE(tree->isIndexValid());
}

// End-to-end reproduction of the exact crash reported in issue #107: a single
// NaN coordinate mixed in among otherwise normal rectangles, small node
// capacity to force an R*-tree split soon after. Before the fix this
// segfaulted inside RstarSplitEntry::compareHigh() on the split triggered by
// a later, unrelated insert; the bad insert itself should now be the one
// that fails, and cleanly.
TEST(RTreeTest, PoisonedInsertAmongNormalDataIsRejectedBeforeAnySplit) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 4, 4, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    double coords[15][4] = {
        {99.72, 93.26, 100.45, 98.25}, {23.61, 39.66, 25.61, 43.04},
        {93.55, 84.63, 95.19, 87.30},  {44.35, 22.96, 47.06, 27.54},
        {45.72, 43.07, 50.42, 46.98},
        // poisoned entry: NaN x-coordinate, as if from a failed upstream parse.
        {std::numeric_limits<double>::quiet_NaN(), 80.28, std::numeric_limits<double>::quiet_NaN(), 82.91},
        {86.50, 82.91, 90.67, 84.35},  {5.92, 67.05, 8.93, 70.44},
        {41.18, 19.76, 42.70, 20.55},  {78.33, 41.25, 78.60, 44.41},
        {66.06, 29.85, 68.35, 31.04},  {7.34, 46.92, 7.91, 51.45},
        {11.95, 52.48, 12.46, 57.07},  {91.04, 29.89, 94.01, 32.77},
        {61.39, 95.65, 62.77, 96.89},
    };

    for (SpatialIndex::id_type i = 0; i < 15; ++i) {
        double plow[2] = { coords[i][0], coords[i][1] };
        double phigh[2] = { coords[i][2], coords[i][3] };
        SpatialIndex::Region r(plow, phigh, 2);

        if (i == 5) {
            EXPECT_THROW(tree->insertData(0, nullptr, r, i), Tools::IllegalArgumentException);
        } else {
            EXPECT_NO_THROW(tree->insertData(0, nullptr, r, i));
        }
    }

    EXPECT_TRUE(tree->isIndexValid());
}

// Follow-up regression for #107 / PR #303: rejecting non-finite *input*
// coordinates isn't enough. Combining otherwise-finite, zero-area point MBRs
// during a split can still overflow the *derived* margin/overlap metrics to
// infinity, which leaves Node::rstarSplit() unable to find a margin/overlap
// smaller than its sentinel initial value. That left splitAxis/splitPoint at
// their uint32_t sentinels, later used to index Region::m_pLow/m_pHigh --
// the same out-of-bounds crash, without any non-finite input ever entering
// the tree.
TEST(RTreeTest, SplitStaysSafeWhenAggregateMarginOverflowsToInfinity) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, 0.7, 4, 4, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier));

    const double m = std::numeric_limits<double>::max() / 4.0;
    double points[5][2] = {
        {-m, -m}, {m, m}, {0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0},
    };

    for (SpatialIndex::id_type i = 0; i < 5; ++i) {
        double p[2] = { points[i][0], points[i][1] };
        SpatialIndex::Region r(p, p, 2);
        EXPECT_NO_THROW(tree->insertData(0, nullptr, r, i));
    }

    EXPECT_TRUE(tree->isIndexValid());
}

// Same root cause as the test above, but on the *other* selection loops that
// pick an entry by comparison. Once a child MBR is wide enough that its area
// overflows to infinity, an enlargement computed as `combined - original`
// becomes inf - inf = NaN. NaN fails every comparison, so:
//
//   * Index::findLeastEnlargement() returned its uint32_t sentinel, which
//     chooseSubtree() then used to index m_pIdentifier (assert in debug
//     builds, out-of-bounds read in release builds),
//   * Index::findLeastOverlap() left `best` null and dereferenced it,
//   * Node::rtreeSplit() left `sel` uninitialized and used it to index,
//   * Node::pickSeeds() left both seed indices unassigned.
//
// Insert enough entries after the overflowing ones to build a tree deep
// enough for chooseSubtree() to reach findLeastEnlargement() (level >= 2).
static void insertOverflowingThenOrdinaryPoints(
    SpatialIndex::RTree::RTreeVariant variant, double fillFactor) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::RTree::createNewRTree(
            *storage, fillFactor, 4, 4, 2, variant, indexIdentifier));

    const double m = std::numeric_limits<double>::max() / 4.0;
    auto insertPoint = [&tree](SpatialIndex::id_type id, double x, double y) {
        double p[2] = { x, y };
        SpatialIndex::Region r(p, p, 2);
        tree->insertData(0, nullptr, r, id);
    };

    // Two points far enough apart that any MBR covering both has an area that
    // overflows to infinity, even though every coordinate is finite.
    EXPECT_NO_THROW(insertPoint(0, -m, -m));
    EXPECT_NO_THROW(insertPoint(1, m, m));

    for (SpatialIndex::id_type i = 2; i < 200; ++i) {
        EXPECT_NO_THROW(insertPoint(i, static_cast<double>(i), static_cast<double>(i)));
    }

    // Keep mixing in overflowing entries so later splits and subtree choices
    // go on seeing non-finite aggregates.
    for (SpatialIndex::id_type i = 200; i < 260; ++i) {
        double s = (i % 3 == 0) ? m : static_cast<double>(i);
        EXPECT_NO_THROW(insertPoint(i, s, -s));
    }

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(RTreeTest, SubtreeChoiceStaysSafeWhenAggregateAreaOverflows) {
    insertOverflowingThenOrdinaryPoints(SpatialIndex::RTree::RV_RSTAR, 0.7);
}

// RV_LINEAR/RV_QUADRATIC go through rtreeSplit()/pickSeeds() instead, and
// require a fill factor below 0.5.
TEST(RTreeTest, LinearSplitStaysSafeWhenAggregateAreaOverflows) {
    insertOverflowingThenOrdinaryPoints(SpatialIndex::RTree::RV_LINEAR, 0.4);
}

TEST(RTreeTest, QuadraticSplitStaysSafeWhenAggregateAreaOverflows) {
    insertOverflowingThenOrdinaryPoints(SpatialIndex::RTree::RV_QUADRATIC, 0.4);
}
