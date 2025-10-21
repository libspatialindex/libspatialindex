#pragma once
#include <spatialindex/SpatialIndex.h>

namespace SpatialIndex { namespace RTree {

class SIDX_DLL IRTreeStatistics : public SpatialIndex::IStatistics {
public:
  virtual uint64_t getSplits() const = 0;
  virtual uint64_t getHits() const = 0;
  virtual uint64_t getMisses() const = 0;
  virtual uint64_t getAdjustments() const = 0;
  virtual uint64_t getQueryResults() const = 0;
  virtual uint32_t getTreeHeight() const = 0;
  virtual uint32_t getNumberOfNodesInLevel(uint32_t l) const = 0;
  ~IRTreeStatistics() override = default;
};

}} // namespace SpatialIndex::RTree

