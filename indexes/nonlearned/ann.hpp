#pragma once

#include "../ann_1.1.2/include/ANN/ANN.h"
#include "../../utils/type.hpp"
#include "../../utils/common.hpp"
#include "../base_index.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <new>
#include <vector>
#include <chrono>

namespace bench { namespace index {

template <size_t Dim>
class ANNKDTree : public BaseIndex {

using Point = point_t<Dim>;
using Box = box_t<Dim>;
using Points = std::vector<point_t<Dim>>;

public:
inline ANNKDTree(Points& points) {
    std::cout << "Construct ANNKDTree..." << std::endl;
    this->num_of_points = points.size();

    auto kdtree_pts = annAllocPts(points.size(), Dim);

    for (size_t i=0; i<points.size(); ++i) {
        for (size_t j=0; j<Dim; ++j) {
            kdtree_pts[i][j] = points[i][j];
        }
    }

    auto start = std::chrono::steady_clock::now();
    index = new ANNkd_tree(kdtree_pts, points.size(), Dim);
    auto end = std::chrono::steady_clock::now();

    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

~ANNKDTree() {
    delete index;
    annClose();
}

inline Points knn_query(Point& q, size_t k, double eps=0.0) {
    std::vector<ANNidx> nn_idx;
    std::vector<ANNdist> nn_dist;
    nn_idx.resize(k);
    nn_dist.resize(k);

    auto start = std::chrono::steady_clock::now();
    index->annkSearch(&q[0], k, &nn_idx[0], &nn_dist[0], eps);
    auto end = std::chrono::steady_clock::now();
    knn_count++;
    knn_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    Points result;
    result.reserve(k);

    for (auto idx : nn_idx) {
        Point p;
        auto temp_pt = index->thePoints()[idx];

        for (size_t i=0; i<Dim; ++i) {
            p[i] = temp_pt[i];
        }
        
        result.emplace_back(p);
    }

    return result;
}

inline size_t count() {
    return this->num_of_points;
}

private:
size_t num_of_points;
ANNkd_tree* index; // internal kd-tree index
};

template <size_t Dim, size_t BucketSize=1>
class ANNBoxDecompositionTree {

using Point = point_t<Dim>;
using Box = box_t<Dim>;
using Points = std::vector<point_t<Dim>>;

public:
inline ANNBoxDecompositionTree(Points& points) {
    std::cout << "Construct ANNBDTree..." << std::endl;
    this->num_of_points = points.size();

    auto bdtree_pts = annAllocPts(points.size(), Dim);

    for (size_t i=0; i<points.size(); ++i) {
        for (size_t j=0; j<Dim; ++j) {
            bdtree_pts[i][j] = points[i][j];
        }
    }

    auto start = std::chrono::steady_clock::now();
    index  = new ANNbd_tree(bdtree_pts, points.size(), Dim, BucketSize);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Construction time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " [ms]" << std::endl;
}

~ANNBoxDecompositionTree() {
    delete index;
    annClose();
}

inline Points knn_query(Point& q, size_t k, double eps=0.0) {
    std::vector<ANNidx> nn_idx;
    std::vector<ANNdist> nn_dist;
    nn_idx.resize(k);
    nn_dist.resize(k);

    index->annkSearch(&q[0], k, &nn_idx[0], &nn_dist[0], eps);

    Points result;
    result.reserve(k);

    for (auto idx : nn_idx) {
        Point p;
        auto temp_pt = index->thePoints()[idx];

        for (size_t i=0; i<Dim; ++i) {
            p[i] = temp_pt[i];
        }

        result.emplace_back(p);
    }

    return result;
}

inline size_t count() {
    return this->num_of_points;
}

private:
size_t num_of_points;
ANNbd_tree* index; // internal box decomposition tree index
};

}
}
