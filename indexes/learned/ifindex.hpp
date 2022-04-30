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
template<size_t Dim, size_t LeafNodeCap>
class IFIndex : public BaseIndex {

using Point = point_t<Dim>;
using Points = std::vector<Point>;
using Box = box_t<Dim>;

// class of Leaf Node
class LeafNode {
    public:
    std::vector<size_t> _ids;
    size_t count;
    // the sort dimension is set to the first dimension by default
    const size_t sort_dim;
    // the maximum prediction error
    size_t max_err;
    // predicted pos = slope * point[sort_dim] + intercept
    double slope;
    double intercept;

    LeafNode(std::vector<size_t> ids, Points& points) : 
    _ids(ids),
    count(ids.size()),
    sort_dim(0) {
        std::vector<std::pair<size_t, double>> id_and_vals;
        std::vector<double> vals, ys;

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

        print_model();
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
using index_rtree_t = bgi::rtree<std::pair<Box, LeafNode>, bgi::linear<32>>;

public:
IFIndex(Points& points) : _points(points) {
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

    // build index rtree
    _rt = new index_rtree_t(idx_data.begin(), idx_data.end());

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

~IFIndex() {
    delete this->_rt;
}

inline size_t count() {
    return this->_points.size();
}

inline size_t index_size() {
    auto num_leaf_nodes = count()/LeafNodeCap + 1;
    
}

Points range_query(Box& box) {

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

};

}
}
