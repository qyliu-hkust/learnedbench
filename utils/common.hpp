#pragma once

#include "type.hpp"
#include "datautils.hpp"
#include <array>
#include <bits/types/struct_rusage.h>
#include <boost/geometry/algorithms/detail/distance/interface.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <deque>
#include <iostream>
#include <map>
#include <queue>
#include <sys/resource.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <chrono>
#include <sys/time.h>

namespace bench { namespace common {

constexpr size_t ipow(size_t base, int exp, size_t result = 1) {
    return exp < 1 ? result : ipow(base*base, exp/2, (exp % 2) ? result*base : result);
}


constexpr size_t find_grid_K(size_t N, size_t dim) {
    size_t temp = 2;
    while (ipow(temp, dim) < (N / 1024)) {
        temp++;
    }
    return temp;
}


template<size_t dim>
inline bool is_in_box(point_t<dim>& p, box_t<dim>& box) {
    // for (size_t d=0; d<dim; ++d) {
    //     if ((p[d] > box.max_corner()[d]) || (p[d] < box.min_corner()[d])) {
    //         return false;
    //     }
    // }

    // return true;
    
    return boost::geometry::covered_by(p, box);
}


template<size_t dim>
inline double eu_dist_square(point_t<dim>& p1, point_t<dim>& p2) {
    double acc = 0;
    for (size_t i=0; i<dim; ++i) {
        auto temp = p1[i] - p2[i];
        acc += temp * temp;
    }
    return acc;
}


template<size_t dim, typename std::enable_if <dim >= 4>::type* = nullptr>
inline double eu_dist(point_t<dim>& p1, point_t<dim>& p2) {
    return std::sqrt(eu_dist_square(p1, p2));
}


template<size_t dim, typename std::enable_if <dim == 3>::type* = nullptr>
inline double eu_dist(point_t<dim>& p1, point_t<dim>& p2) {
    return std::hypot(p1[0]-p2[0], p1[1]-p2[1], p1[2]-p2[2]);
}


template<size_t dim, typename std::enable_if <dim == 2>::type* = nullptr>
inline double eu_dist(point_t<dim>& p1, point_t<dim>& p2) {
    return std::hypot(p1[0]-p2[0], p1[1]-p2[1]);
}


template<size_t dim>
inline void print_point(point_t<dim>& p, bool is_endl=true) {
    std::cout << std::fixed;
    std::cout << "Point(";
    for (size_t i=0; i<dim-1; ++i) {
        std::cout << p[i] << ", ";
    }
    if (is_endl)
        std::cout << p[dim-1] << ")" << std::endl;
    else
        std::cout << p[dim-1] << ")";
}


template<size_t dim>
inline void print_box(box_t<dim>& box) {
    std::cout << "Box(";
    print_point(box.min_corner(), false);
    std::cout << ", ";
    print_point(box.max_corner(), false);
    std::cout << ")" <<std::endl;
}


template<size_t dim>
inline void print_points(vec_of_point_t<dim>& points) {
    for (auto p : points) {
        print_point(p);
    }
}


template<size_t dim>
inline void print_knn_result(point_t<dim>& q, vec_of_point_t<dim>& knn) {
    std::sort(knn.begin(), knn.end(), 
        [&q](point_t<dim>& p1, point_t<dim>& p2) -> bool { 
            return eu_dist_square(p1, q) > eu_dist_square(p2, q); });

    for (auto p : knn) {
        print_point(p);
        std::cout << "dist=" << std::sqrt(eu_dist_square(p, q)) << std::endl;
    }
}


// Boost rtree visitor to compute rtree statistics
template <typename Value, typename Options, typename Box, typename Allocators>
struct statistics : public boost::geometry::index::detail::rtree::visitor<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag, true>::type
{
    typedef typename boost::geometry::index::detail::rtree::internal_node<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag>::type internal_node;
    typedef typename boost::geometry::index::detail::rtree::leaf<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag>::type leaf;

    inline statistics()
        : level(0)
        , levels(1) // count root
        , nodes(0)
        , leaves(0)
        , values(0)
        , values_min(0)
        , values_max(0)
        , tree_size(0)
    {}

    inline void operator()(internal_node const& n)
    {
        typedef typename boost::geometry::index::detail::rtree::elements_type<internal_node>::type elements_type;
        elements_type const& elements = boost::geometry::index::detail::rtree::elements(n);
        
        ++nodes; // count node
        tree_size += sizeof(internal_node);

        size_t const level_backup = level;
        ++level;
        
        levels += level++ > levels ? 1 : 0; // count level (root already counted)
                
        for (typename elements_type::const_iterator it = elements.begin();
            it != elements.end(); ++it)
        {
            boost::geometry::index::detail::rtree::apply_visitor(*this, *it->second);
        }
        
        level = level_backup;
    }

    inline void operator()(leaf const& n)
    {   
        typedef typename boost::geometry::index::detail::rtree::elements_type<leaf>::type elements_type;
        elements_type const& elements = boost::geometry::index::detail::rtree::elements(n);

        ++leaves; // count leaves
        tree_size += sizeof(leaf);
        
        std::size_t const v = elements.size();
        // count values spread per node and total
        values_min = (std::min)(values_min == 0 ? v : values_min, v);
        values_max = (std::max)(values_max, v);
        values += v;
    }
    
    std::size_t level;
    std::size_t levels;
    std::size_t nodes;
    std::size_t leaves;
    std::size_t values;
    std::size_t values_min;
    std::size_t values_max;
    std::size_t tree_size;
};

// apply the rtree visitor
template <typename Rtree> inline
size_t get_boost_rtree_statistics(Rtree const& tree) {
    typedef boost::geometry::index::detail::rtree::utilities::view<Rtree> RTV;
    RTV rtv(tree);

    statistics<
        typename RTV::value_type,
        typename RTV::options_type,
        typename RTV::box_type,
        typename RTV::allocators_type
    > stats_v;

    rtv.apply_visitor(stats_v);

    return stats_v.tree_size;
}



// struct BaseBench {
//     inline static std::string data_path = "../data/";
//     inline static std::string real_data_path = "../data/real/";
//     inline static std::string syn_data_path_n = "../data/synthetic/N/";
//     inline static std::string syn_data_path_dim = "../data/synthetic/Dim/";
//     inline static std::string syn_data_path_default = "../data/synthetic/Default/";

//     inline static size_t FS_N = 3680126;
//     inline static size_t TORONTO_N = 21567172;
//     inline static size_t OSM_N = 62734869;
//     inline static ssize_t TPC_N = 41995307;

//     static void load_fs(vec_of_point_t<2>& points) {
//         std::cout << "=====================================" << std::endl;
//         std::cout << "Read data: foursquare" << std::endl;
//         bench::utils::read_points(points, real_data_path + "fs", FS_N);
//     }

//     static void load_toronto(vec_of_point_t<3>& points) {
//         std::cout << "=====================================" << std::endl;
//         std::cout << "Read data: toronto" << std::endl;
//         bench::utils::read_points(points, real_data_path + "toronto", TORONTO_N);
//     }

//     static void load_osm(vec_of_point_t<2>& points) {
//         std::cout << "=====================================" << std::endl;
//         std::cout << "Read data: osm" << std::endl;
//         bench::utils::read_points(points, real_data_path + "osm-china", OSM_N);
//     }

//     static void load_tpch(vec_of_point_t<4>& points) {
//         std::cout << "=====================================" << std::endl;
//         std::cout << "Read data: tpch"  << std::endl;
//         bench::utils::read_points(points, real_data_path + "tpc", TPC_N);
//     }

//     enum flag {DEFAULT, VARY_N, VARY_DIM};

//     template<size_t Dim>
//     static void load_synthetic(vec_of_point_t<Dim>& points, const std::string& dist, const size_t N, const flag f) {
//         std::ostringstream ss;

//         if (f == flag::DEFAULT) {
//             ss << syn_data_path_default;
//         } else if (f == flag::VARY_N) {
//             ss << syn_data_path_n;
//         } else {
//             ss << syn_data_path_dim;
//         }
        
//         ss << dist << "_" << N << "m_" << Dim << "_1";
//         std::string fname = ss.str();
        
//         std::cout << "=====================================" << std::endl;
//         std::cout << "Read data from " << fname << std::endl;

//         bench::utils::read_points(points, fname, N * 1000000);
//     }

//     // sample queries from data
//     // sample point queries
//     template<size_t dim>
//     static std::vector<point_t<dim>> sample_point_queries(vec_of_point_t<dim>& points, size_t s=100) {
//         // seed the generator
//         std::mt19937 gen(0); 
//         std::uniform_int_distribution<> uint_dist(0, points.size()-1);

//         // generate random indices
//         std::vector<point_t<dim>> samples;
//         samples.reserve(s);

//         for (size_t i=0; i<s; ++i) {
//             auto idx = uint_dist(gen);
//             samples.emplace_back(points[idx]);
//         }

//         return samples;
//     }


//     // sample knn queries
//     template<size_t dim>
//     static std::vector<knn_t<dim>> sample_knn_queries(vec_of_point_t<dim>& points, size_t s=100) {
//         int ks[5] = {1, 10, 100, 500, 1000};
//         auto query_points = sample_point_queries(points, s);

//         std::vector<knn_t<dim>> knn_queries;
//         knn_queries.reserve(5 * s);

//         for (int i=0; i<5; ++i) {
//             for (auto point : query_points) {
//                 knn_queries.emplace_back(std::make_pair(point, ks[i]));
//             }
//         }

//         return knn_queries;
//     }


//     template<size_t dim>
//     static std::pair<point_t<dim>, point_t<dim>> min_and_max(std::vector<point_t<dim>>& points) {
//         point_t<dim> mins;
//         point_t<dim> maxs;

//         // boundaries of each dimension
//         std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
//         std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

//         for (size_t i=0; i<dim; ++i) {
//             for (auto& p : points) {
//                 mins[i] = std::min(p[i], mins[i]);
//                 maxs[i] = std::max(p[i], maxs[i]);
//             }
//         }

//         return std::make_pair(mins, maxs);
//     }

//     // sample range queries
//     template<size_t dim>
//     static std::vector<box_t<dim>> sample_range_queries(vec_of_point_t<dim>& points, size_t s=5) {
//         double selectivities[5] = {0.001, 0.01, 0.05, 0.1, 0.2};
//         auto corner_points = sample_point_queries(points, s);
        
//         std::pair<point_t<dim>, point_t<dim>> min_max = min_and_max(points);

//         std::vector<box_t<dim>> range_queries;
//         range_queries.reserve(5 * s);
        
//         for (int i=0; i<5; ++i) {
//             for (auto& point : corner_points) {
//                 point_t<dim> another_corner;
//                 for (size_t d=0; d<dim; ++d) {
//                     // this is a roughly estimation of selectivity by assuming 
//                     // uniform distribution and dimension independence
//                     double step = (min_max.second[d] - min_max.first[d]) * std::pow(selectivities[i], 1.0/dim);
//                     another_corner[d] = point[d] + step;
//                 }
//                 box_t<dim> box(point, another_corner);
//                 range_queries.emplace_back(box);
//             }
//         }
        
//         return range_queries;
//     }

//     template<class Index, size_t Dim>
//     static void batch_point_quries(Index& index, vec_of_point_t<Dim>& query_points) {
//         auto start = std::chrono::steady_clock::now();
//         for (auto q_point : query_points) {
//             index.point_query(q_point);
//         }
//         auto end = std::chrono::steady_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//         std::cout << "Avg. Time: " << (duration * 1.0) / query_points.size() << std::endl;
//     }

//     template<class Index, size_t Dim>
//     static void batch_knn_queries(Index& index, std::vector<knn_t<Dim>>& knn_queries) {
//         std::map<size_t, double> total_time_k;
//         std::map<size_t, size_t> count_k;

//         for (auto knn : knn_queries) {
//             auto start = std::chrono::steady_clock::now();
//             index.knn_query(knn.first, knn.second);
//             auto end = std::chrono::steady_clock::now();
//             auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//             if (total_time_k.count(knn.second)) {
//                 total_time_k[knn.second] += duration;
//                 count_k[knn.second] += 1;
//             } else {
//                 total_time_k[knn.second] = duration;
//                 count_k[knn.second] = 1;
//             }
//         }

//         for (const auto& p : total_time_k) {
//             std::cout << "k=" << p.first << " Avg. Time: " << p.second / count_k[p.first] << std::endl;
//         }
//     }

//     template<class Index, size_t Dim>
//     static void batch_range_queries(Index& index, std::vector<box_t<Dim>>& range_queries) {
//         std::map<size_t, long> time_count;

//         for (auto& box : range_queries) {
//             auto start = std::chrono::steady_clock::now();
//             auto range_count = index.range_query(box).size();
//             auto end = std::chrono::steady_clock::now();
//             auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//             time_count[range_count] = duration;
//         }

//         // time for each range query has been ordered by result size
//         for (const auto& p : time_count) {
//             std::cout << "Sel=" << (p.first * 1.0) / index.count() << " Time: " << p.second << std::endl;
//         }
//     }
// };

}
}
