/*
 * Codex wrote this support layer for the gtest port of the legacy index tests.
 * It replaces the shell-script harness with shared in-process fixtures:
 * simple rectangle math, visitors, in-memory storage helpers, bulk-load
 * streams, and exhaustive reference checks for RTree, MVRTree, and TPRTree
 * behavior.
 */

#ifndef NATIVE_TEST_SUPPORT_H
#define NATIVE_TEST_SUPPORT_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <spatialindex/SpatialIndex.h>

#include "test.h"

namespace sidx_test {

static const int kDelete = 0;
static const int kInsert = 1;
static const int kQuery = 2;

// A tiny rectangle model used as the independent oracle for two-dimensional
// tests. It deliberately does not call into libspatialindex geometry methods.
struct Rect {
    Rect() : xmin(0.0), ymin(0.0), xmax(0.0), ymax(0.0) {}

    Rect(double x1, double y1, double x2, double y2)
        : xmin(std::min(x1, x2)),
          ymin(std::min(y1, y2)),
          xmax(std::max(x1, x2)),
          ymax(std::max(y1, y2)) {}

    bool intersects(const Rect& other) const {
        return !(xmin > other.xmax || xmax < other.xmin ||
                 ymin > other.ymax || ymax < other.ymin);
    }

    double minDist(const Rect& other) const {
        double dist = 0.0;

        if (other.xmax < xmin) dist += std::pow(xmin - other.xmax, 2.0);
        else if (other.xmin > xmax) dist += std::pow(other.xmin - xmax, 2.0);

        if (other.ymax < ymin) dist += std::pow(ymin - other.ymax, 2.0);
        else if (other.ymin > ymax) dist += std::pow(other.ymin - ymax, 2.0);

        return dist;
    }

    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

inline SpatialIndex::Region regionFromRect(const Rect& rect) {
    double low[2] = {rect.xmin, rect.ymin};
    double high[2] = {rect.xmax, rect.ymax};
    return SpatialIndex::Region(low, high, 2);
}

inline SpatialIndex::Point pointFromRectLow(const Rect& rect) {
    double low[2] = {rect.xmin, rect.ymin};
    return SpatialIndex::Point(low, 2);
}

class IdVisitor : public SpatialIndex::IVisitor {
public:
    void visitNode(const SpatialIndex::INode& node) override {
        if (node.isLeaf()) leafIO++;
        else indexIO++;
    }

    void visitData(const SpatialIndex::IData& data) override {
        ids.push_back(data.getIdentifier());
    }

    void visitData(std::vector<const SpatialIndex::IData*>& data) override {
        ASSERT_GE(data.size(), 2u);
        pairs.push_back(std::make_pair(data[0]->getIdentifier(), data[1]->getIdentifier()));
    }

    size_t indexIO{0};
    size_t leafIO{0};
    std::vector<SpatialIndex::id_type> ids;
    std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> > pairs;
};

inline void sortIds(std::vector<SpatialIndex::id_type>& ids) {
    std::sort(ids.begin(), ids.end());
}

inline void sortPairs(std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> >& pairs) {
    std::sort(pairs.begin(), pairs.end());
}

inline void expectSameIds(
    std::vector<SpatialIndex::id_type> expected,
    std::vector<SpatialIndex::id_type> actual) {
    sortIds(expected);
    sortIds(actual);
    EXPECT_EQ(expected, actual);
}

inline void expectSamePairs(
    std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> > expected,
    std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> > actual) {
    sortPairs(expected);
    sortPairs(actual);
    EXPECT_EQ(expected, actual);
}

inline std::unique_ptr<SpatialIndex::IStorageManager> memoryStorage() {
    return std::unique_ptr<SpatialIndex::IStorageManager>(
        SpatialIndex::StorageManager::createNewMemoryStorageManager());
}

inline std::string rectData(const Rect& rect) {
    std::ostringstream os;
    os << rect.xmin << "," << rect.ymin << "," << rect.xmax << "," << rect.ymax;
    return os.str();
}

inline void insertRect(
    SpatialIndex::ISpatialIndex& tree,
    SpatialIndex::id_type id,
    const Rect& rect) {
    SpatialIndex::Region r = regionFromRect(rect);
    std::string data = rectData(rect);
    tree.insertData(
        static_cast<uint32_t>(data.size() + 1),
        reinterpret_cast<const uint8_t*>(data.c_str()),
        r,
        id);
}

inline bool deleteRect(
    SpatialIndex::ISpatialIndex& tree,
    SpatialIndex::id_type id,
    const Rect& rect) {
    SpatialIndex::Region r = regionFromRect(rect);
    return tree.deleteData(r, id);
}

inline std::vector<SpatialIndex::id_type> intersectingIds(
    const std::map<SpatialIndex::id_type, Rect>& active,
    const Rect& query) {
    std::vector<SpatialIndex::id_type> result;
    for (std::map<SpatialIndex::id_type, Rect>::const_iterator it = active.begin();
         it != active.end();
         ++it) {
        if (query.intersects(it->second)) result.push_back(it->first);
    }
    return result;
}

inline std::vector<SpatialIndex::id_type> nearestIds(
    const std::map<SpatialIndex::id_type, Rect>& active,
    const Rect& query,
    size_t k) {
    std::vector<std::pair<double, SpatialIndex::id_type> > distances;
    for (std::map<SpatialIndex::id_type, Rect>::const_iterator it = active.begin();
         it != active.end();
         ++it) {
        distances.push_back(std::make_pair(it->second.minDist(query), it->first));
    }

    std::sort(distances.begin(), distances.end());

    std::vector<SpatialIndex::id_type> result;
    double kthDistance = 0.0;
    for (size_t i = 0; i < distances.size(); ++i) {
        if (i >= k && distances[i].first > kthDistance) break;
        result.push_back(distances[i].second);
        kthDistance = distances[i].first;
    }
    return result;
}

inline std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> > selfJoinPairs(
    const std::map<SpatialIndex::id_type, Rect>& active,
    const Rect& query) {
    std::vector<std::pair<SpatialIndex::id_type, SpatialIndex::id_type> > result;
    for (std::map<SpatialIndex::id_type, Rect>::const_iterator first = active.begin();
         first != active.end();
         ++first) {
        if (!query.intersects(first->second)) continue;
        for (std::map<SpatialIndex::id_type, Rect>::const_iterator second = active.begin();
             second != active.end();
             ++second) {
            if (first->first == second->first) continue;
            if (query.intersects(second->second) && first->second.intersects(second->second)) {
                result.push_back(std::make_pair(first->first, second->first));
            }
        }
    }
    return result;
}

class RTreeDataStream : public SpatialIndex::IDataStream {
public:
    explicit RTreeDataStream(const std::vector<std::pair<SpatialIndex::id_type, Rect> >& data)
        : m_data(data), m_next(0) {}

    SpatialIndex::IData* getNext() override {
        if (!hasNext()) return nullptr;

        const std::pair<SpatialIndex::id_type, Rect>& entry = m_data[m_next++];
        SpatialIndex::Region r = regionFromRect(entry.second);
        double low[2] = {entry.second.xmin, entry.second.ymin};
        // The bulk loader takes ownership of IData instances returned by the
        // stream, matching the ownership model used by the legacy test tools.
        return new SpatialIndex::RTree::Data(
            sizeof(double),
            reinterpret_cast<uint8_t*>(low),
            r,
            entry.first);
    }

    bool hasNext() override {
        return m_next < m_data.size();
    }

    uint32_t size() override {
        return static_cast<uint32_t>(m_data.size());
    }

    void rewind() override {
        m_next = 0;
    }

private:
    std::vector<std::pair<SpatialIndex::id_type, Rect> > m_data;
    size_t m_next;
};

struct MvrEntry {
    SpatialIndex::id_type id;
    Rect rect;
    double start;
    double end;
};

struct MvrOperation {
    int op;
    SpatialIndex::id_type id;
    Rect rect;
    double time;
    double queryStart;
    double queryEnd;
};

inline MvrOperation mvrInsert(SpatialIndex::id_type id, const Rect& rect, double time) {
    MvrOperation op = {kInsert, id, rect, time, 0.0, 0.0};
    return op;
}

inline MvrOperation mvrDelete(SpatialIndex::id_type id, const Rect& rect, double time) {
    MvrOperation op = {kDelete, id, rect, time, 0.0, 0.0};
    return op;
}

inline MvrOperation mvrQuery(const Rect& rect, double start, double end) {
    MvrOperation op = {kQuery, 0, rect, 0.0, start, end};
    return op;
}

inline SpatialIndex::TimeRegion timeRegion(
    const Rect& rect,
    double start,
    double end) {
    double low[2] = {rect.xmin, rect.ymin};
    double high[2] = {rect.xmax, rect.ymax};
    return SpatialIndex::TimeRegion(low, high, start, end, 2);
}

inline bool intersectsInTime(const MvrEntry& entry, const MvrOperation& query) {
    // MVR deletion closes the version interval; queries intersect entries
    // whose live interval overlaps the requested time window.
    if (entry.end <= query.queryStart || entry.start >= query.queryEnd) return false;
    return entry.rect.intersects(query.rect);
}

inline void mvrInsertData(
    SpatialIndex::ISpatialIndex& tree,
    std::vector<MvrEntry>& history,
    SpatialIndex::id_type id,
    const Rect& rect,
    double time) {
    SpatialIndex::TimeRegion r = timeRegion(rect, time, time);
    tree.insertData(0, nullptr, r, id);
    MvrEntry entry = {id, rect, time, std::numeric_limits<double>::max()};
    history.push_back(entry);
}

inline void mvrDeleteData(
    SpatialIndex::ISpatialIndex& tree,
    std::vector<MvrEntry>& history,
    SpatialIndex::id_type id,
    const Rect& rect,
    double time) {
    SpatialIndex::TimeRegion r = timeRegion(rect, time, time);
    ASSERT_TRUE(tree.deleteData(r, id));

    for (std::vector<MvrEntry>::reverse_iterator it = history.rbegin();
         it != history.rend();
         ++it) {
        if (it->id == id && it->end == std::numeric_limits<double>::max()) {
            it->end = time;
            return;
        }
    }

    FAIL() << "Could not find live MVR entry " << id;
}

inline std::vector<SpatialIndex::id_type> mvrExpectedIds(
    const std::vector<MvrEntry>& history,
    const MvrOperation& query) {
    std::vector<SpatialIndex::id_type> result;
    for (std::vector<MvrEntry>::const_iterator it = history.begin();
         it != history.end();
         ++it) {
        if (intersectsInTime(*it, query)) result.push_back(it->id);
    }
    return result;
}

inline std::vector<SpatialIndex::id_type> queryIds(
    SpatialIndex::ISpatialIndex& tree,
    const SpatialIndex::IShape& query) {
    IdVisitor visitor;
    tree.intersectsWithQuery(query, visitor);
    return visitor.ids;
}

struct TprPoint {
    SpatialIndex::id_type id;
    double ax;
    double ay;
    double vx;
    double vy;
    double refTime;
};

struct TprOperation {
    int op;
    SpatialIndex::id_type id;
    double currentTime;
    double referenceTime;
    double ax;
    double vx;
    double ay;
    double vy;
};

inline TprOperation tprInsert(
    SpatialIndex::id_type id,
    double currentTime,
    double ax,
    double ay,
    double vx,
    double vy) {
    TprOperation op = {kInsert, id, currentTime, currentTime, ax, vx, ay, vy};
    return op;
}

inline TprOperation tprDelete(
    const TprPoint& point,
    double currentTime) {
    TprOperation op = {
        kDelete, point.id, currentTime, point.refTime, point.ax, point.vx, point.ay, point.vy};
    return op;
}

inline TprOperation tprQuery(
    double start,
    double end,
    double xmin,
    double ymin,
    double xmax,
    double ymax) {
    TprOperation op = {kQuery, 0, start, end, xmin, xmax, ymin, ymax};
    return op;
}

inline SpatialIndex::MovingRegion movingPointRegion(
    double ax,
    double ay,
    double vx,
    double vy,
    double start,
    double end) {
    double low[2] = {ax, ay};
    double high[2] = {ax, ay};
    double vlow[2] = {vx, vy};
    double vhigh[2] = {vx, vy};
    Tools::Interval interval(start, end);
    return SpatialIndex::MovingRegion(low, high, vlow, vhigh, interval, 2);
}

inline SpatialIndex::MovingRegion movingQueryRegion(
    const TprOperation& query) {
    double low[2] = {query.ax, query.ay};
    double high[2] = {query.vx, query.vy};
    double vlow[2] = {0.0, 0.0};
    double vhigh[2] = {0.0, 0.0};
    Tools::Interval interval(query.currentTime, query.referenceTime);
    return SpatialIndex::MovingRegion(low, high, vlow, vhigh, interval, 2);
}

inline double tprX(const TprPoint& point, double time) {
    return point.ax + point.vx * (time - point.refTime);
}

inline double tprY(const TprPoint& point, double time) {
    return point.ay + point.vy * (time - point.refTime);
}

inline size_t clipCode(double x, double y, const Rect& rect) {
    size_t code = 0;
    if (y > rect.ymax) code |= 0x1;
    else if (y < rect.ymin) code |= 0x2;
    if (x > rect.xmax) code |= 0x4;
    else if (x < rect.xmin) code |= 0x8;
    return code;
}

inline bool segmentIntersectsRect(
    double x0,
    double y0,
    double x1,
    double y1,
    const Rect& rect) {
    size_t c0 = clipCode(x0, y0, rect);
    size_t c1 = clipCode(x1, y1, rect);

    // Cohen-Sutherland clipping is enough for the TPR oracle because each
    // moving point traces a single line segment across the query interval.
    for (;;) {
        if ((c0 | c1) == 0) return true;
        if ((c0 & c1) != 0) return false;

        size_t c = c0 ? c0 : c1;
        double x;
        double y;

        if (c & 0x1) {
            x = x0 + (x1 - x0) * (rect.ymax - y0) / (y1 - y0);
            y = rect.ymax;
        } else if (c & 0x2) {
            x = x0 + (x1 - x0) * (rect.ymin - y0) / (y1 - y0);
            y = rect.ymin;
        } else if (c & 0x4) {
            x = rect.xmax;
            y = y0 + (y1 - y0) * (rect.xmax - x0) / (x1 - x0);
        } else {
            x = rect.xmin;
            y = y0 + (y1 - y0) * (rect.xmin - x0) / (x1 - x0);
        }

        if (c == c0) {
            x0 = x;
            y0 = y;
            c0 = clipCode(x0, y0, rect);
        } else {
            x1 = x;
            y1 = y;
            c1 = clipCode(x1, y1, rect);
        }
    }
}

inline bool tprIntersects(const TprPoint& point, const TprOperation& query) {
    Rect rect(query.ax, query.ay, query.vx, query.vy);
    // Compare the point's swept path over the query interval with the static
    // query rectangle used by these tests.
    return segmentIntersectsRect(
        tprX(point, query.currentTime),
        tprY(point, query.currentTime),
        tprX(point, query.referenceTime),
        tprY(point, query.referenceTime),
        rect);
}

inline std::vector<SpatialIndex::id_type> tprExpectedIds(
    const std::map<SpatialIndex::id_type, TprPoint>& active,
    const TprOperation& query) {
    std::vector<SpatialIndex::id_type> result;
    for (std::map<SpatialIndex::id_type, TprPoint>::const_iterator it = active.begin();
         it != active.end();
         ++it) {
        if (tprIntersects(it->second, query)) result.push_back(it->first);
    }
    return result;
}

inline void tprInsertData(
    SpatialIndex::ISpatialIndex& tree,
    std::map<SpatialIndex::id_type, TprPoint>& active,
    const TprOperation& op) {
    SpatialIndex::MovingRegion r = movingPointRegion(
        op.ax,
        op.ay,
        op.vx,
        op.vy,
        op.currentTime,
        std::numeric_limits<double>::max());
    tree.insertData(0, nullptr, r, op.id);
    TprPoint point = {op.id, op.ax, op.ay, op.vx, op.vy, op.currentTime};
    active[op.id] = point;
}

inline void tprDeleteData(
    SpatialIndex::ISpatialIndex& tree,
    std::map<SpatialIndex::id_type, TprPoint>& active,
    const TprOperation& op) {
    SpatialIndex::MovingRegion r = movingPointRegion(
        op.ax,
        op.ay,
        op.vx,
        op.vy,
        op.referenceTime,
        op.currentTime);
    ASSERT_TRUE(tree.deleteData(r, op.id));
    active.erase(op.id);
}

} // namespace sidx_test

#endif // NATIVE_TEST_SUPPORT_H
