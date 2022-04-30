#pragma once

#include "../../utils/type.hpp"
#include "../../utils/common.hpp"
#include "../base_index.hpp"

#include <boost/geometry/index/parameters.hpp>
#include <boost/geometry/index/predicates.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <cstddef>
#include <iterator>
#include <chrono>


#ifdef HEAP_PROFILE
#include <gperftools/heap-profiler.h>
#endif


namespace bgi = boost::geometry::index;

namespace bench { namespace index {

template<size_t dim, size_t MaxElements=128>
class RTree : public BaseIndex {

using Point = point_t<dim>;
using Box = box_t<dim>;
using Points = std::vector<point_t<dim>>;
using rtree_t = bgi::rtree<Point, bgi::linear<MaxElements>>;

public:
inline RTree(Points& points) {
    std::cout << "Construct R-tree " << "Dim=" << dim << " MaxElements=" << MaxElements << std::endl;

    auto start = std::chrono::steady_clock::now();

#ifdef HEAP_PROFILE
HeapProfilerStart("rtree");
#endif

    // construct r-tree using packing algorithm
    rtree = new rtree_t(points.begin(), points.end());

#ifdef HEAP_PROFILE
HeapProfilerDump("final");
HeapProfilerStop();
#endif

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

~RTree() {
    delete rtree;
}


inline Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();
    Points return_values;
    rtree->query(bgi::covered_by(box), std::back_inserter(return_values));
    auto end = std::chrono::steady_clock::now();
    range_count++;
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return return_values;
}


inline Points knn_query(Point& q, unsigned int k) {
    auto start = std::chrono::steady_clock::now();
    Points return_values;
    rtree->query(bgi::nearest(q, k), std::back_inserter(return_values));
    auto end = std::chrono::steady_clock::now();
    knn_count++;
    knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return return_values;
}

inline size_t count() {
    return rtree->size();
} 

private:
rtree_t* rtree;
};

template<size_t dim, size_t MaxElements=128>
class RStarTree : BaseIndex {

using Point = point_t<dim>;
using Box = box_t<dim>;
using Points = std::vector<point_t<dim>>;
using rtree_t = bgi::rtree<Point, bgi::rstar<MaxElements>>;

public:
RStarTree(Points& points) {
    std::cout << "Construct R*-tree " << "Dim=" << dim << " MaxElements=" << MaxElements << std::endl;

    auto start = std::chrono::steady_clock::now();

#ifdef HEAP_PROFILE
HeapProfilerStart("rstartree");
#endif

    rtree = new rtree_t();

    for (auto& p : points) {
        rtree->insert(p);
    }

#ifdef HEAP_PROFILE
HeapProfilerDump("final");
HeapProfilerStop();
#endif

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

~RStarTree() {
    delete this->rtree;
}

inline Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();
    Points return_values;
    rtree->query(bgi::covered_by(box), std::back_inserter(return_values));
    auto end = std::chrono::steady_clock::now();
    range_count++;
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return return_values;
}

inline Points knn_query(Point& q, unsigned int k) {
    auto start = std::chrono::steady_clock::now();
    Points return_values;
    rtree->query(bgi::nearest(q, k), std::back_inserter(return_values));
    auto end = std::chrono::steady_clock::now();
    knn_count++;
    knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return return_values;
}

inline size_t count() {
    return rtree->size();
} 


private:
rtree_t* rtree;

};


}
}
