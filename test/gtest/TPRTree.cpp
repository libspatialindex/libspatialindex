/*
 * Codex wrote this gtest port of the legacy TPRTree shell-script tests.
 * The port keeps the moving-object insert/delete/query behavior in process,
 * using GoogleTest assertions and local motion calculations instead of
 * running generated command files through shell scripts.
 */

#include "NativeTestSupport.h"

TEST(TPRTreeTest, SplitIntersectionQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::TPRTree::createNewTPRTree(
            *storage, 0.7, 20, 20, 2, SpatialIndex::TPRTree::TPRV_RSTAR, 20, indexIdentifier));

    // Active moving points are mirrored outside the tree so the expected
    // answer can be calculated from straight-line motion over the query time.
    std::map<SpatialIndex::id_type, sidx_test::TprPoint> active;

    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(1, 0.0, 0.0, 0.0, 0.1, 0.1));
    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(2, 0.0, 0.8, 0.1, -0.05, 0.05));
    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(3, 0.0, 0.5, 0.9, 0.0, -0.1));

    sidx_test::TprOperation query = sidx_test::tprQuery(2.0, 4.0, 0.2, 0.2, 0.45, 0.45);
    SpatialIndex::MovingRegion queryRegion = sidx_test::movingQueryRegion(query);
    sidx_test::expectSameIds(sidx_test::tprExpectedIds(active, query), sidx_test::queryIds(*tree, queryRegion));

    EXPECT_TRUE(tree->isIndexValid());
}

TEST(TPRTreeTest, MixedIntersectionQueriesMatchExhaustiveSearch) {
    std::unique_ptr<SpatialIndex::IStorageManager> storage = sidx_test::memoryStorage();
    SpatialIndex::id_type indexIdentifier;
    std::unique_ptr<SpatialIndex::ISpatialIndex> tree(
        SpatialIndex::TPRTree::createNewTPRTree(
            *storage, 0.7, 10, 10, 2, SpatialIndex::TPRTree::TPRV_RSTAR, 20, indexIdentifier));

    // This mirrors a mixed TPR command stream: query the initial moving
    // objects, delete one at a later time, insert another, then query again.
    std::map<SpatialIndex::id_type, sidx_test::TprPoint> active;

    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(1, 0.0, 0.0, 0.0, 0.1, 0.1));
    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(2, 0.0, 0.7, 0.2, -0.05, 0.0));

    sidx_test::TprOperation firstQuery = sidx_test::tprQuery(2.0, 4.0, 0.2, 0.2, 0.45, 0.45);
    SpatialIndex::MovingRegion firstRegion = sidx_test::movingQueryRegion(firstQuery);
    sidx_test::expectSameIds(
        sidx_test::tprExpectedIds(active, firstQuery),
        sidx_test::queryIds(*tree, firstRegion));

    sidx_test::TprOperation deleteFirst = sidx_test::tprDelete(active[1], 5.0);
    sidx_test::tprDeleteData(*tree, active, deleteFirst);
    sidx_test::tprInsertData(*tree, active, sidx_test::tprInsert(4, 5.0, 0.3, 0.8, 0.0, -0.1));

    sidx_test::TprOperation secondQuery = sidx_test::tprQuery(6.0, 8.0, 0.25, 0.45, 0.35, 0.65);
    SpatialIndex::MovingRegion secondRegion = sidx_test::movingQueryRegion(secondQuery);
    sidx_test::expectSameIds(
        sidx_test::tprExpectedIds(active, secondQuery),
        sidx_test::queryIds(*tree, secondRegion));

    EXPECT_TRUE(tree->isIndexValid());
}
