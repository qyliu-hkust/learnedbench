#pragma once

#include "./dkm.hpp"
#include "../utils/common.hpp"
#include "../utils/type.hpp"
#include "../pgm/pgm_index.hpp"
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

namespace bench { namespace index {

// eps is the error bound for the underlying 1-D learned index
template<size_t dim, size_t eps>
class MLIndex {

using Point = point_t<dim>;
using Entry = std::pair<Point, double>;
using Box = box_t<dim>;
using Points = std::vector<point_t<dim>>;

public:
MLIndex(Points& points, int p): p(p) {
    // call k-means with fixed random seed
    dkm::clustering_parameters<double> config(p);
    config.set_random_seed(0);
    auto clusters = dkm::kmeans_lloyd(points, config); 
    
    // fill means vector
    means.reserve(p);
    for (const auto& mean : std::get<0>(clusters)) {
        means.emplace_back(mean);
    }

    // fill radii vector
    radii.resize(p);
    std::fill(radii.begin(), radii.begin(), 0);
    for (int i=0; i<points.size(); ++i) {
        auto pid = std::get<1>(clusters)[i];
        auto dist = bench::common::eu_dist(means[pid], points[i]);
        radii[pid] = radii[pid] < dist ? dist : radii[pid];
    }

    // note radii stores partial sums
    for (int i=1; i<p; ++i) {
        radii[i] += radii[i-1];
    }

    // construct learned index on projected values
    std::vector<double> projections;
    projections.reserve(points.size());
    for (const auto& p : points) {
        projections.emplace_back(project(p));
    }

    // 
    std::sort(projections.begin(), projections.end());
    index = construct_index(projections);
}   

// projection function
inline double project(Point& p) {
    auto j = 0;
    auto min_dist = std::numeric_limits<double>::max();
    for (int i=0; i<p; ++i) {
        auto temp_dist = bench::common::eu_dist(means[i], p);
        if (temp_dist < min_dist) {
            min_dist = temp_dist;
            j = i;
        }
    }

    return j == 0 ? min_dist : radii[j-1] + min_dist;
}


const int& get_p() const {
    return p;
}

const Points& get_means() const {
    return means;
}

const std::vector<double>& get_radii() const {
    return radii;
}

// Points dist_range_query(Points& q, double d) {
    
// }


// Points range_query(Box& box) {

// }


// Points knn_query(Point& q, size_t k) {

// }



private:
// the partition number
int p; 

// raw data
std::vector<Entry> data;

// vec of means of each partition
Points means; 

// vec of radius of each partition (partial sum for efficiency)
std::vector<double> radii;

// underlying pgm learned index
pgm::PGMIndex<double, eps> index; 

pgm::PGMIndex<double, eps> construct_index(std::vector<double>& data) {
    pgm::PGMIndex<double, eps> index(data);
    return index;
}

};

}}
