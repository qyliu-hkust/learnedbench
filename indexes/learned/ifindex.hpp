#pragma once

#include <cstddef>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <chrono>

#include <boost/geometry/index/rtree.hpp>
#include <boost/math/statistics/linear_regression.hpp>

#include "../base_index.hpp"
#include "../../utils/type.hpp"

namespace bgi = boost::geometry::index;
using boost::math::statistics::simple_ordinary_least_squares;

namespace bench { namespace index {

// implementation of the IF-Index by augumenting boost rtree
// the original paper uses linear interpolation whose error is generally large
// instead, we train a simple linear regression model as a trade-off
template<size_t Dim, size_t LeafNodeCap=2000, size_t MaxElements=32, size_t sort_dim=0>
class IFIndex : public BaseIndex {

using Point = point_t<Dim>;
using Points = std::vector<Point>;
using Box = box_t<Dim>;

public:
// class of Leaf Node
class LeafNode {
    public:
    std::vector<size_t> _ids;
    Points _local_points;
    size_t count;
    // the maximum prediction error
    size_t max_err;
    // predicted pos = slope * point[sort_dim] + intercept
    double slope;
    double intercept;

    LeafNode(std::vector<size_t> ids, Points& points) : _ids(ids), count(ids.size()) {
        std::vector<std::pair<size_t, double>> id_and_vals;
        std::vector<double> vals, ys;

        _local_points.reserve(count);

        id_and_vals.reserve(count);
        vals.resize(this->count);
        ys.resize(this->count);

        for (auto id : _ids) {
            id_and_vals.emplace_back(std::make_pair(id, points[id][sort_dim]));
        }

        // sort the array by sort_dim
        std::sort(id_and_vals.begin(), id_and_vals.end(), 
            [](auto p1, auto p2){ return std::get<1>(p1) < std::get<1>(p2); });

        // un-tie pairs
        for (size_t i=0; i<count; ++i) {
            _ids[i] = std::get<0>(id_and_vals[i]);
            vals[i] = std::get<1>(id_and_vals[i]);
            ys[i] = static_cast<double>(i);
        }

        // update local points in the bucket
        for (auto id : _ids) {
            _local_points.emplace_back(points[id]);
        }
        
        // train a linear regression model using ordinary least square
        auto [a, b] = simple_ordinary_least_squares(vals, ys);
        intercept = a;
        slope = b;

        // compute the max error
        max_err = 0;
        for (size_t i=0; i<count; ++i) {
            auto pred = predict(vals[i]);
            auto err = (pred >= i) ? (pred - i) : (i - pred);
            max_err = (err > max_err) ? err : max_err;
        }
    }

    void print_model() {
        std::cout << "f(x) = " << this->intercept << " + " << this->slope << " * x Max Error: " << this->max_err << std::endl;
    }

    inline size_t predict(double val) {
        double guess = slope * val + intercept;
        if (guess < 0) {
            return 0;
        }
        if (guess > (count - 1)) {
            return count - 1;
        }
        return static_cast<size_t>(guess);
    }
};


using pack_rtree_t = bgi::rtree<std::pair<Point, size_t>, bgi::linear<LeafNodeCap>>;
using index_rtree_t = bgi::rtree<std::pair<Box, LeafNode>, bgi::linear<MaxElements>>;


IFIndex(Points& points) : _points(points) {
    std::cout << "Construct IFIndex (on Rtree) " << "LeafNodeCap=" << LeafNodeCap 
            << " MaxElements=" << MaxElements << " sort_dim=" << sort_dim << std::endl;

    auto start = std::chrono::steady_clock::now();

    std::vector<std::pair<Point, size_t>> point_with_id;
    point_with_id.reserve(points.size());
    size_t cnt = 0;
    for (auto & p : points) {
        point_with_id.emplace_back(p, (cnt++));
    }

    // run STR algorithm to bulk-load points
    pack_rtree_t temp_rt(point_with_id.begin(), point_with_id.end());

    // group leaf nodes
    std::vector<std::pair<Box, LeafNode>> idx_data;
    idx_data.reserve((points.size() / LeafNodeCap) + 1);
    cnt = 0;
    std::vector<size_t> temp_ids;
    temp_ids.reserve(LeafNodeCap);
    for (auto it=temp_rt.begin(); it!=temp_rt.end(); ++it) {
        temp_ids.emplace_back(std::get<1>(*it));
        if ((++cnt) % LeafNodeCap == 0) {
            idx_data.emplace_back(compute_mbr(temp_ids, points), LeafNode(temp_ids, points));
            temp_ids.clear();
        }
    }

    if (temp_ids.size() != 0) {
        idx_data.emplace_back(compute_mbr(temp_ids, points), LeafNode(temp_ids, points));
        temp_ids.clear();
    }

    // build index rtree
    _rt = new index_rtree_t(idx_data.begin(), idx_data.end());

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Build Time: " << get_build_time() << " [ms]" << std::endl;
    std::cout << "Index Size: " << index_size() << " Bytes" << std::endl;
}

~IFIndex() {
    delete this->_rt;
}

inline size_t count() {
    return this->_points.size();
}

// the index size is the sum of rtree and all identifiers
inline size_t index_size() {
    auto rt_size = bench::common::get_boost_rtree_statistics(*(this->_rt));
    return rt_size + count() * sizeof(size_t);
}

Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();

    Points result;
    // for leaf nodes covered by the query box
    // directly insert points to the result set
    for (auto it=_rt->qbegin(bgi::covered_by(box)); it!=_rt->qend(); ++it) {
        for (auto p : std::get<1>(*it)._local_points) {
            result.emplace_back(p);
        }
    }

    // for leaf nodes overlaps the query box 
    // check whether the points are in the query range
    for (auto it=_rt->qbegin(bgi::overlaps(box)); it!=_rt->qend(); ++it) {
        const LeafNode& leaf = std::get<1>(*it);
        auto [lo, hi] = search_leaf(leaf, box);
        for (auto i=lo; i<=hi; ++i) {
            Point temp_p = leaf._local_points[i];
            if (bench::common::is_in_box(temp_p, box)) {
                result.emplace_back(temp_p);
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    range_count ++;

    return result;
}

private:
Points& _points;
index_rtree_t* _rt;

inline Box compute_mbr(std::vector<size_t>& ids, Points& points) {
    Point mins, maxs;
    std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
    std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

    for (size_t i=0; i<Dim; ++i) {
        for (auto id : ids) {
            mins[i] = std::min(points[id][i], mins[i]);
            maxs[i] = std::max(points[id][i], maxs[i]);
        }
    }

    return Box(mins, maxs);
}

inline size_t predict(const LeafNode& leaf, double val) {
    double guess = leaf.slope * val + leaf.intercept;
    if (guess < 0) {
        return 0;
    }
    if (guess > (leaf.count - 1)) {
        return leaf.count - 1;
    }
    return static_cast<size_t>(guess);
}

inline std::pair<size_t, size_t> search_leaf(const LeafNode& leaf, Box& box) {
    size_t pred_min = predict(leaf, box.min_corner()[sort_dim]);
    size_t pred_max = predict(leaf, box.max_corner()[sort_dim]);
    size_t lo = (leaf.max_err > pred_min) ? 0 : (pred_min - leaf.max_err);
    size_t hi = ((pred_max + leaf.max_err + 1) > leaf.count) ? (leaf.count - 1) : (pred_max + leaf.max_err);
    return std::make_pair(lo, hi);
}

};

}
}
