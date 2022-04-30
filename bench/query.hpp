#pragma once

#include <cstddef>
#include <random>
#include <map>
#include <algorithm>

#include "../utils/type.hpp"
#include "../indexes/nonlearned/fullscan.hpp"


namespace bench { namespace query {

// sample queries from data
// sample point queries
template<size_t dim>
static std::vector<point_t<dim>> sample_point_queries(vec_of_point_t<dim>& points, size_t s=100) {
    // seed the generator
    std::mt19937 gen(0); 
    std::uniform_int_distribution<> uint_dist(0, points.size()-1);

    // generate random indices
    std::vector<point_t<dim>> samples;
    samples.reserve(s);

    for (size_t i=0; i<s; ++i) {
        auto idx = uint_dist(gen);
        samples.emplace_back(points[idx]);
    }

    return samples;
}


// sample knn queries
// k is in range {1, 10, 100, 500, 1000, 10000}
// each k has s=100 samples
template<size_t dim>
static std::map<size_t, vec_of_point_t<dim>> sample_knn_queries(vec_of_point_t<dim>& points, size_t s=100) {
    int ks[6] = {1, 10, 100, 500, 1000, 10000};
    auto query_points = sample_point_queries(points, s);

    std::map<size_t, vec_of_point_t<dim>> knn_queries;

    for (auto k : ks) {
        for (auto& point : query_points) {
            knn_queries[k].emplace_back(point);
        }
    }

    return knn_queries;
}


template<size_t dim>
static std::pair<point_t<dim>, point_t<dim>> min_and_max(std::vector<point_t<dim>>& points) {
    point_t<dim> mins;
    point_t<dim> maxs;

    // boundaries of each dimension
    std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
    std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

    for (size_t i=0; i<dim; ++i) {
        for (auto& p : points) {
            mins[i] = std::min(p[i], mins[i]);
            maxs[i] = std::max(p[i], maxs[i]);
        }
    }

    return std::make_pair(mins, maxs);
}


// sample range queries
// selectivity = range_count(q_box) / N
// for each selectivity we generate s=10 random boxes roughly match the selectivity
template<size_t dim>
static std::vector<std::pair<box_t<dim>, size_t>> sample_range_queries(vec_of_point_t<dim>& points, size_t s=10) {
    double selectivities[5] = {0.001, 0.01, 0.05, 0.1, 0.2};
    auto corner_points = sample_point_queries(points, s);
    bench::index::FullScan<dim> fs(points);
    
    std::pair<point_t<dim>, point_t<dim>> min_max = min_and_max(points);

    std::vector<std::pair<box_t<dim>, size_t>> range_queries;
    range_queries.reserve(5 * s);
    
    for (int i=0; i<5; ++i) {
        for (auto& point : corner_points) {
            point_t<dim> another_corner;
            for (size_t d=0; d<dim; ++d) {
                // this is a roughly estimation of selectivity by assuming 
                // uniform distribution and dimension independence
                double step = (min_max.second[d] - min_max.first[d]) * std::pow(selectivities[i], 1.0/dim);
                // make sure the generated box is within the data range
                another_corner[d] = std::min(point[d] + step, min_max.second[d]);
            }
            box_t<dim> box(point, another_corner);
            range_queries.emplace_back(box, fs.range_query(box).size());
        }
    }
    
    return range_queries;
}


template<class Index, size_t Dim>
static void batch_knn_queries(Index& index, std::map<size_t, vec_of_point_t<Dim>>& knn_queries) {
    int ks[6] = {1, 10, 100, 500, 1000, 10000};

    for (auto k : ks) {
        for (auto& q_point : knn_queries[k]) {
            index.knn_query(q_point, k);
        }
        std::cout << "k=" << k << " Avg. Time: " << index.get_avg_knn_time() << " [us]" << std::endl;
        index.reset_timer();
    }

}


template<class Index, size_t Dim>
static void batch_range_queries(Index& index, std::vector<std::pair<box_t<Dim>, size_t>> range_queries) {
    // pair (range_cnt, time)
    std::vector<std::pair<size_t, long>> range_time;
    range_time.reserve(range_queries.size());
    
    for (auto& box : range_queries) {
        index.range_query(box.first);
        range_time.emplace_back(box.second, index.get_range_time());
        index.reset_timer();
    }

    // sort by range_cnt
    std::sort(range_time.begin(), range_time.end(), 
        [](auto p1, auto p2) {
            return p1.first < p2.first;
        });

    // print time ordered by selectivity
    size_t bucket_size = range_time.size() / 5;
    size_t N = index.count();
    std::vector<std::pair<size_t, long>> temp;
    size_t cnt = 0;

    std::cout << "Sel=[";
    for (auto rt : range_time) {
        std::cout << rt.first / (1.0*N) << ",";
    }
    std::cout << "]" << std::endl;

    for (auto rt : range_time) {
        if (cnt == bucket_size) {
            double sel_lo = temp.front().first / (1.0*N);
            double sel_hi = temp.back().first / (1.0*N);
            double avg = 0.0;
            for (auto& tt : temp) {
                avg += tt.second;
            }
            std::cout << "Sel=[" << sel_lo << ", " << sel_hi << "]" << " Avg. Time: " << avg/bucket_size << " [us]" << std::endl;
            temp.clear();
            cnt = 0;
        } else {
            temp.emplace_back(rt);
            cnt ++;
        }
    }

    double sel_lo = temp.front().first / (1.0*N);
    double sel_hi = temp.back().first / (1.0*N);
    double avg = 0.0;
    for (auto& tt : temp) {
        avg += tt.second;
    }
    std::cout << "Sel=[" << sel_lo << ", " << sel_hi << "]" << " Avg. Time: " << avg/bucket_size << " [us]" << std::endl;
}

}
}


