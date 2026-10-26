/*
 * Codex wrote this gtest port of the legacy MVRTree shell-script tests.
 * The old scripts exercised insert/delete/query sequences through command
 * line tools; this file performs those temporal operations directly against
 * an in-memory MVRTree and checks each query against a local version history.
 */

#include "NativeTestSupport.h"

TEST(MVRTreeTest, SplitIntersectionQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::MVRTree::createNewMVRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::MVRTree::RV_RSTAR, indexIdentifier));

    // Keep the version intervals outside the tree so each temporal query can
    // be checked against a simple exhaustive timeline.
    std::vector<sidx_test::MvrEntry> history;
    sidx_test::Rect r1(0.0, 0.0, 0.3, 0.3);
    sidx_test::Rect r2(0.2, 0.2, 0.5, 0.5);
    sidx_test::Rect r3(0.7, 0.7, 0.9, 0.9);

    sidx_test::mvrInsertData(*tree, history, 1, r1, 0.0);
    sidx_test::mvrInsertData(*tree, history, 2, r2, 5.0);
    sidx_test::mvrInsertData(*tree, history, 3, r3, 10.0);
    sidx_test::mvrDeleteData(*tree, history, 1, r1, 12.0);

    // These queries cover a live overlap, a deleted interval, and a later
    // object that starts after the first insert/delete sequence.
    std::vector<sidx_test::MvrOperation> queries;
    queries.push_back(sidx_test::mvrQuery(sidx_test::Rect(0.1, 0.1, 0.35, 0.35), 6.0, 8.0));
    queries.push_back(sidx_test::mvrQuery(sidx_test::Rect(0.1, 0.1, 0.35, 0.35), 13.0, 15.0));
    queries.push_back(sidx_test::mvrQuery(sidx_test::Rect(0.75, 0.75, 0.85, 0.85), 11.0, 12.0));

    for (std::vector<sidx_test::MvrOperation>::const_iterator it = queries.begin();
         it != queries.end();
         ++it) {
        SpatialIndex::TimeRegion query = sidx_test::timeRegion(it->rect, it->queryStart, it->queryEnd);
        sidx_test::expectSameIds(sidx_test::mvrExpectedIds(history, *it), sidx_test::queryIds(*tree, query));
    }

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(MVRTreeTest, MixedIntersectionQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::MVRTree::createNewMVRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::MVRTree::RV_RSTAR, indexIdentifier));

    // The second MVR test keeps the old script's mixed shape: query, delete,
    // insert, and query again against the updated temporal state.
    std::vector<sidx_test::MvrEntry> history;
    sidx_test::Rect r1(0.0, 0.0, 0.25, 0.25);
    sidx_test::Rect r2(0.2, 0.2, 0.45, 0.45);
    sidx_test::Rect r3(0.5, 0.5, 0.8, 0.8);

    sidx_test::mvrInsertData(*tree, history, 10, r1, 0.0);
    sidx_test::mvrInsertData(*tree, history, 11, r2, 3.0);

    sidx_test::MvrOperation firstQuery =
        sidx_test::mvrQuery(sidx_test::Rect(0.15, 0.15, 0.3, 0.3), 4.0, 6.0);
    SpatialIndex::TimeRegion firstRegion =
        sidx_test::timeRegion(firstQuery.rect, firstQuery.queryStart, firstQuery.queryEnd);
    sidx_test::expectSameIds(
        sidx_test::mvrExpectedIds(history, firstQuery),
        sidx_test::queryIds(*tree, firstRegion));

    sidx_test::mvrDeleteData(*tree, history, 10, r1, 7.0);
    sidx_test::mvrInsertData(*tree, history, 12, r3, 8.0);

    sidx_test::MvrOperation secondQuery =
        sidx_test::mvrQuery(sidx_test::Rect(0.0, 0.0, 0.6, 0.6), 9.0, 10.0);
    SpatialIndex::TimeRegion secondRegion =
        sidx_test::timeRegion(secondQuery.rect, secondQuery.queryStart, secondQuery.queryEnd);
    sidx_test::expectSameIds(
        sidx_test::mvrExpectedIds(history, secondQuery),
        sidx_test::queryIds(*tree, secondRegion));

    EXPECT_TRUE(tree->isIndexValid());
}

// Port of the RTree regressions for #107 / #303 to the MVR-tree, which has
// the same selection loops (see the comments on the RTree tests).

TEST(MVRTreeTest, InsertingNonFiniteValuesThrowsInsteadOfCorruptingTree) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::MVRTree::createNewMVRTree(
            *storage, 0.7, 10, 10, 2, SpatialIndex::MVRTree::RV_RSTAR, indexIdentifier));

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();

    double nan_low[2] = {nan, 1.0}, nan_high[2] = {2.0, 2.0};
    EXPECT_THROW(
        tree->insertData(0, nullptr, SpatialIndex::TimeRegion(nan_low, nan_high, 0.0, 1.0, 2), 1),
        Tools::IllegalArgumentException);

    double inf_low[2] = {1.0, 1.0}, inf_high[2] = {inf, 2.0};
    EXPECT_THROW(
        tree->insertData(0, nullptr, SpatialIndex::TimeRegion(inf_low, inf_high, 0.0, 1.0, 2), 2),
        Tools::IllegalArgumentException);

    // A NaN start time passes the "older than current time" check, since
    // every comparison with NaN is false.
    double low[2] = {0.0, 0.0}, high[2] = {1.0, 1.0};
    EXPECT_THROW(
        tree->insertData(0, nullptr, SpatialIndex::TimeRegion(low, high, nan, 1.0, 2), 3),
        Tools::IllegalArgumentException);

    // Rejected input must not leave partial state behind.
    EXPECT_NO_THROW(
        tree->insertData(0, nullptr, SpatialIndex::TimeRegion(low, high, 0.0, 1.0, 2), 4));
    EXPECT_TRUE(tree->isIndexValid());
}

// Two finite points far enough apart that any MBR covering both has an area
// that overflows to infinity, then enough ordinary points to grow the tree
// deep enough for chooseSubtree() to reach findLeastEnlargement() and
// findLeastOverlap(), mixing overflowing points back in. (10 is the smallest
// node capacity the MVR-tree accepts.) Before the fix this
// ran into the uint32_t sentinel / null / uninitialized indices left by the
// selection loops when every comparison fails on NaN.
static void mvrInsertOverflowingThenOrdinaryPoints(
    SpatialIndex::MVRTree::MVRTreeVariant variant, double fillFactor) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::MVRTree::createNewMVRTree(
            *storage, fillFactor, 10, 10, 2, variant, indexIdentifier));

    const double m = std::numeric_limits<double>::max() / 4.0;
    auto insertPoint = [&tree](SpatialIndex::id_type id, double x, double y) {
        double p[2] = {x, y};
        const double t = static_cast<double>(id);
        tree->insertData(0, nullptr, SpatialIndex::TimeRegion(p, p, t, t + 1.0, 2), id);
    };

    EXPECT_NO_THROW(insertPoint(0, -m, -m));
    EXPECT_NO_THROW(insertPoint(1, m, m));

    for (SpatialIndex::id_type i = 2; i < 200; ++i) {
        EXPECT_NO_THROW(insertPoint(i, static_cast<double>(i), static_cast<double>(i)));
    }

    for (SpatialIndex::id_type i = 200; i < 260; ++i) {
        double s = (i % 3 == 0) ? m : static_cast<double>(i);
        EXPECT_NO_THROW(insertPoint(i, s, -s));
    }

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(MVRTreeTest, SubtreeChoiceStaysSafeWhenAggregateAreaOverflows) {
    mvrInsertOverflowingThenOrdinaryPoints(SpatialIndex::MVRTree::RV_RSTAR, 0.7);
}

// RV_LINEAR/RV_QUADRATIC go through rtreeSplit()/pickSeeds() instead, and
// require a fill factor below 0.5.
TEST(MVRTreeTest, LinearSplitStaysSafeWhenAggregateAreaOverflows) {
    mvrInsertOverflowingThenOrdinaryPoints(SpatialIndex::MVRTree::RV_LINEAR, 0.4);
}

TEST(MVRTreeTest, QuadraticSplitStaysSafeWhenAggregateAreaOverflows) {
    mvrInsertOverflowingThenOrdinaryPoints(SpatialIndex::MVRTree::RV_QUADRATIC, 0.4);
}
