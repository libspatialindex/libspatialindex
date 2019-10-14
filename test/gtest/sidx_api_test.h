#ifndef SIDX_API_TEST_H
#define SIDX_API_TEST_H

#include <spatialindex/capi/sidx_api.h>
#include <cstring>

#include "test.h"

double min[] = {0.5, 0.5};
double max[] = {0.5, 0.5};
int nDims = 2;
int nId = 1;

char pszData[5];

class SidxApiRTreeTest : public testing::Test {
  protected:
    // virtual void SetUp() will be called before each test is run.  You
    // should define it if you need to initialize the variables.
    // Otherwise, this can be skipped.
    void SetUp() override {
      memset(pszData, '\0', sizeof(pszData));
#ifdef WIN32
      strcpy_s(pszData, sizeof(pszData), "TEST");
#else
      strcpy(pszData, "TEST");
#endif
      IndexPropertyH props = IndexProperty_Create();
      IndexProperty_SetIndexType(props, RT_RTree);
      IndexProperty_SetIndexStorage(props, RT_Memory);
      idx = Index_Create(props);
      IndexProperty_Destroy(props); 
      Index_InsertData(idx, nId, min, max, nDims, (uint8_t *)pszData, strlen(pszData) + 1);
    }

    // virtual void TearDown() will be called after each test is run.
    // You should define it if there is cleanup work to do.  Otherwise,
    // you don't have to provide it.
    //
    void TearDown() override {
      Index_Destroy(idx);
    }

    IndexH idx;
};

TEST_F(SidxApiRTreeTest, valid) {
  ASSERT_EQ(1, Index_IsValid(idx));
}

TEST_F(SidxApiRTreeTest, intersects_id) {
  uint64_t nResults;
  int64_t* items;  
  Index_Intersects_id(idx, min, max, nDims, &items, &nResults);
  EXPECT_EQ(1, nResults);
  EXPECT_EQ(nId, items[0]);   
  free(items);  
}

TEST_F(SidxApiRTreeTest, intersects_nearest_id) {
  uint64_t nResults;
  int64_t* items;  
  Index_NearestNeighbors_id(idx, min, max, nDims, &items, &nResults);
  EXPECT_EQ(1, nResults);
  EXPECT_EQ(nId, items[0]);   
  free(items);  
}

TEST_F(SidxApiRTreeTest, intersects_obj) {
  uint64_t nResults;
  IndexItemH* items;
  char* pszRes = nullptr;
  uint64_t len = 0;  
  Index_Intersects_obj(idx, min, max, nDims, &items, &nResults);
  ASSERT_EQ(1, nResults);
  IndexItem_GetData(items[0], (uint8_t **)&pszRes, &len);
  EXPECT_EQ(0, strcmp(pszData, pszRes));
  free(pszRes);
  Index_DestroyObjResults(items, (uint32_t) nResults);  
}

TEST_F(SidxApiRTreeTest, intersects_nearest_obj) {
  uint64_t nResults;
  IndexItemH* items;
  char* pszRes = nullptr;
  uint64_t len = 0;  
  Index_NearestNeighbors_obj(idx, min, max, nDims, &items, &nResults);
  ASSERT_EQ(1, nResults);
  IndexItem_GetData(items[0], (uint8_t **)&pszRes, &len);
  EXPECT_EQ(0, strcmp(pszData, pszRes));
  free(pszRes);
  Index_DestroyObjResults(items, (uint32_t) nResults); 
}

TEST_F(SidxApiRTreeTest, intersects_count) {
  uint64_t nResults;
  Index_Intersects_count(idx, min, max, nDims, &nResults);
  EXPECT_EQ(1, nResults);
}

TEST_F(SidxApiRTreeTest, intersects_bounds) {
  double* pMin;
  double* pMax;
  uint32_t nResDims;
  Index_GetBounds(idx, &pMin, &pMax, &nResDims);
  ASSERT_EQ(nDims, nResDims);
  EXPECT_EQ(min[0], pMin[0]);
  EXPECT_EQ(min[1], pMin[1]);
  EXPECT_EQ(max[0], pMax[0]);
  EXPECT_EQ(max[1], pMax[1]);
  free(pMin);
  free(pMax);
}

TEST_F(SidxApiRTreeTest, contains_obj) {
    uint64_t nResults;
    IndexItemH* items;
    char* pszRes = nullptr;
    uint64_t len = 0;
    Index_Contains_obj(idx, min, max, nDims, &items, &nResults);
    ASSERT_EQ(1, nResults);
    IndexItem_GetData(items[0], (uint8_t **)&pszRes, &len);
    EXPECT_EQ(0, strcmp(pszData, pszRes));
    free(pszRes);
    Index_DestroyObjResults(items, (uint32_t) nResults);
}

TEST_F(SidxApiRTreeTest, contains_id) {
    uint64_t nResults;
    int64_t* items;
    Index_Contains_id(idx, min, max, nDims, &items, &nResults);
    EXPECT_EQ(1, nResults);
    EXPECT_EQ(nId, items[0]);
    free(items);
}

TEST_F(SidxApiRTreeTest, contains_count) {
    uint64_t nResults;
    Index_Contains_count(idx, min, max, nDims, &nResults);
    EXPECT_EQ(1, nResults);
}

#endif // SIDX_API_TEST_H
