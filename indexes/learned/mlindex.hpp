#pragma once

#include "dkm.hpp"
#include "../../utils/common.hpp"
#include "../../utils/type.hpp"
#include "../base_index.hpp"
#include "../pgm/pgm_index.hpp"

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>
#include <chrono>
#include <queue>
#include <cmath>

namespace bench { namespace index {

// eps is the error bound for the underlying 1-D learned index
// p is the partition number (input of the kmeans algorithm)
template<size_t dim, size_t eps=64, size_t p=50>
class MLIndex : public BaseIndex {

using Point = point_t<dim>;
using Box = box_t<dim>;
using Points = std::vector<point_t<dim>>;

public:
MLIndex(Points& points) {
    std::cout << "Construct ML-Index: " << "partition=" << p << " eps=" << eps << std::endl;

    auto start = std::chrono::steady_clock::now();

    // call k-means with fixed random seed
    dkm::clustering_parameters<double> config(p);
    // fix the random seed for reproduction
    config.set_random_seed(0);
    config.set_max_iteration(20);
    // find kmeans clusters
    auto clusters = dkm::kmeans_lloyd(points, config); 
    
    // fill means vector
    means.reserve(p);
    for (const auto& mean : std::get<0>(clusters)) {
        means.emplace_back(mean);
    }

    // fill radii vector
    std::fill(this->radii.begin(), this->radii.end(), 0.0);

    for (size_t i=0; i<points.size(); ++i) {
        auto pid = std::get<1>(clusters)[i];
        auto dist = bench::common::eu_dist(means[pid], points[i]);
        radii[pid] = std::max(radii[pid], dist);
    }

    // fill the offsets vector
    std::fill(this->offsets.begin(), this->offsets.end(), 0.0);

    for (size_t i=1; i<p; ++i) {
        offsets[i] = radii[i-1];
    }

    for (size_t i=1; i<p; ++i) {
        offsets[i] += offsets[i-1];
    }

    // construct learned index on projected values
    std::vector<std::pair<Point, double>> point_with_projection;
    std::vector<double> projections;

    point_with_projection.reserve(points.size());
    projections.reserve(points.size());
    this->_data.reserve(points.size());

    for (auto& point : points) {
        point_with_projection.emplace_back(point, project(point));
    }

    std::sort(point_with_projection.begin(), point_with_projection.end(), 
        [](auto p1, auto p2) { 
            return std::get<1>(p1) < std::get<1>(p2); 
        });

    for (auto& pp : point_with_projection) {
        this->_data.emplace_back(std::get<0>(pp));
        projections.emplace_back(std::get<1>(pp));
    }
    
    this->_pgm = new pgm::PGMIndex<double, eps>(projections);

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Build Time: " << get_build_time() << " [ms]" << std::endl;
    std::cout << "Index Size: " << index_size() << " Bytes" << std::endl;
}

~MLIndex() {
    delete this->_pgm;
}

inline size_t count() {
    return this->_data.size();
}

inline size_t index_size() {
    return p * (2*sizeof(double) + sizeof(Point)) + _pgm->size_in_bytes() + count() * sizeof(size_t);
}

Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();

    auto min_corner = box.min_corner();
    auto max_corner = box.max_corner();

    Point center;
    for (size_t i=0; i<dim; ++i) {
        center[i] = (max_corner[i] + min_corner[i]) / 2.0;
    }
    double radius = bench::common::eu_dist(min_corner, max_corner) / 2.0;

    Points candidates;

    dist_search(candidates, center, radius);

    Points results;
    for (auto& cand : candidates) {
        if (bench::common::is_in_box(cand, box)) {
            results.emplace_back(cand);
        }
    }

    auto end = std::chrono::steady_clock::now();
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    range_count ++;

    return results;
}

Points knn_query(Point& point, size_t k) {
    auto start = std::chrono::steady_clock::now();

    // initial search distance
    size_t p_id = find_closest_center(point);
    double r = this->radii[p_id] * std::pow((p * k)/(count() * 1.0), 1.0/dim);

    Points temp_result;
    while (1) {
        dist_search(temp_result, point, r);

        // k results found
        if (temp_result.size() >= k) {
            break;
        }

        r *= 2;
        temp_result.clear();
    }

    auto end = std::chrono::steady_clock::now();
    knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    knn_count ++;

    std::sort(temp_result.begin(), temp_result.end(), 
        [&](auto& p1, auto p2) { 
            return bench::common::eu_dist_square(p1, point) < bench::common::eu_dist_square(p2, point); 
        });
    
    Points result(temp_result.begin(), temp_result.begin()+k);
    return result;
}

// search points in a circle cenerted at q_point with radius=dist
inline void dist_search(Points& results, Point& q_point, double dist) {
    assert(dist > 0);

    // search each partition
    for (size_t i=0; i<p; ++i) {
        partition_search(results, q_point, dist, i);
    }
}

// map a distance range query to 1-D intervals on each partition
inline void partition_search(Points& results, Point& q_point, double radius, size_t partition_id) {
    double partition_radius = this->radii[partition_id];
    double dist_to_center = bench::common::eu_dist(q_point, this->means[partition_id]);

    double lo=0.0, hi=0.0;
    
    if (dist_to_center < (radius + partition_radius) && dist_to_center > std::fabs(radius - partition_radius)) {
        if (dist_to_center < radius) {
            // case 1: two circle intersect and data center is in the search circle
            lo = this->offsets[partition_id];
            hi = this->offsets[partition_id] + this->radii[partition_id];
        } else if (dist_to_center < partition_radius) {
            // case 2: two circle intersect and search center is in the data partition
            lo = this->offsets[partition_id] + (dist_to_center - radius);
            hi = this->offsets[partition_id] + this->radii[partition_id];
        } else {
            // case 3: two circle intersect and no center is in another partition
            lo = this->offsets[partition_id] + (dist_to_center - radius);
            hi = this->offsets[partition_id] + this->radii[partition_id];
        }
    }

    if (dist_to_center < (partition_radius - radius)) {
        // case 4: data partition covers the search circle
        lo = this->offsets[partition_id] + (dist_to_center - radius);
        hi = this->offsets[partition_id] + (dist_to_center + radius);
    }

    if (dist_to_center < (radius - partition_radius)) {
        // case 5: search circle covers the data partition
        lo = this->offsets[partition_id];
        hi = this->offsets[partition_id] + this->radii[partition_id];
    }

    if (dist_to_center > (partition_radius + radius)) {
        // case 6: two circles are disjoint and nothing to do
        return;
    }
    
    // query the index
    assert(hi > lo);
    double radius_square = radius * radius;
    auto range_lo = this->_pgm->search(lo);
    auto range_hi = this->_pgm->search(hi);
    auto it_lo = _data.begin() + range_lo.lo;
    auto it_hi = _data.begin() + range_hi.hi;

    for (auto it=it_lo; it!=it_hi; ++it) {
        // validate whether the result is within the given distance threshold radius
        if (bench::common::eu_dist_square(*it, q_point) < radius_square) {
            results.emplace_back(*it);
        }
    }
}

private:
// raw data
Points _data;

// vec of means of each partition
Points means; 

// vec of radius of each partition
std::array<double, p> radii;

// vec of offsets
std::array<double, p> offsets;

// underlying pgm learned index
pgm::PGMIndex<double, eps>* _pgm; 

// find the closest partition center to a query point
inline size_t find_closest_center(Point& point) {
    size_t j = 0;
    double min_dist = std::numeric_limits<double>::max();
    for (size_t i=0; i<p; ++i) {
        double temp_dist = bench::common::eu_dist(means[i], point);
        if (temp_dist < min_dist) {
            min_dist = temp_dist;
            j = i;
        }
    }

    return j;
}

// the ML-Index projection function
inline double project(Point& point) {
    size_t j = 0;
    double min_dist = std::numeric_limits<double>::max();
    for (size_t i=0; i<p; ++i) {
        double temp_dist = bench::common::eu_dist(means[i], point);
        if (temp_dist < min_dist) {
            min_dist = temp_dist;
            j = i;
        }
    }

    return offsets[j] + min_dist;
}

};

}}
