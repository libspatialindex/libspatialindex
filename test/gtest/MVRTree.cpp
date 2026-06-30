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
