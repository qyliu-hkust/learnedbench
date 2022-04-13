#pragma once


#include "../utils/type.hpp"
#include "../utils/common.hpp"
#include <boost/geometry/index/parameters.hpp>
#include <boost/geometry/index/predicates.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <cstddef>
#include <iterator>
#include <chrono>

namespace bgi = boost::geometry::index;

namespace bench { namespace index {

template<size_t dim, size_t MaxElements=256>
class RTree {

using Point = point_t<dim>;
using Box = box_t<dim>;
using Points = std::vector<point_t<dim>>;
using rtree_t = bgi::rtree<Point, bgi::linear<MaxElements>>;

public:
inline RTree(Points& points) {
    std::cout << "Construct R-tree" << std::endl;

    bench::common::dump_mem_usage();
    auto start = std::chrono::steady_clock::now();

    // construct r-tree using packing algorithm
    rtree = new rtree_t(points.begin(), points.end());

    auto end = std::chrono::steady_clock::now();
    bench::common::dump_mem_usage();
    std::cout << "Construction time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " [ms]" << std::endl;
}

~RTree() {
    delete rtree;
}


inline Points range_query(Box& box) {
    Points return_values;
    rtree->query(bgi::covered_by(box), std::back_inserter(return_values));
    return return_values;
}


inline Points knn_query(Point& q, unsigned int k) {
    Points return_values;
    rtree->query(bgi::nearest(q, k), std::back_inserter(return_values));
    return return_values;
}

inline size_t count() {
    return rtree->size();
} 

private:
bgi::rtree<Point, bgi::linear<MaxElements>>* rtree;
};


}}
