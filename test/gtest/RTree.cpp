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
