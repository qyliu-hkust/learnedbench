#pragma once

#include <cstddef>
#include <utility>
#include <cstdint>
#include <chrono>
#include <cmath>

#include "../base_index.hpp"
#include "../../utils/type.hpp"
#include "../../utils/common.hpp"
#include "../pgm/pgm_index.hpp"
#include "../pgm/pgm_index_variants.hpp"
#include "../pgm/morton_nd.hpp"


namespace bench { namespace index {

// Epsilon: the error bound of the underlying 1-D learned index
template<size_t Dim, size_t Epsilon=64>
class ZMIndex : public BaseIndex {

using Point = point_t<Dim>;
using Points = std::vector<Point>;
using Box = box_t<Dim>;
using Index = pgm::MultidimensionalPGMIndex<Dim, size_t, Epsilon>;
using morton = mortonnd::MortonNDBmi<Dim, uint64_t>;
using value_type = decltype(morton::Decode(0));

public:

ZMIndex(Points& points) : _data(points) {
    std::cout << "Construct ZM-Index " << "Epsilon=" << Epsilon << std::endl;

    auto start = std::chrono::steady_clock::now();

    // boundaries of each dimension
    std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
    std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

    for (size_t i=0; i<Dim; ++i) {
        for (auto& p : points) {
            mins[i] = std::min(p[i], mins[i]);
            maxs[i] = std::max(p[i], maxs[i]);
        }
    }

    // the grid resolution to calculate the Z-value is set to N^{1/d}
    this->resolution = static_cast<size_t>(pow(_data.size(), 1.0/Dim));
    
    // widths of each dimension
    for (size_t i=0; i<Dim; ++i) {
        widths[i] = (maxs[i] - mins[i]) / this->resolution;
    }

    std::vector<value_type> tuples;
    tuples.reserve(points.size());
    for (auto& p: points) {
        tuples.emplace_back(a2t(p));
    }
    
    pgm_idx = new Index(tuples.begin(), tuples.end());

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Build Time: " << get_build_time() << " [ms]" << std::endl;
    std::cout << "Index Size: " << index_size() << " Bytes" << std::endl;
}

~ZMIndex() {
    delete this->pgm_idx;
}

Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();

    auto min_tup = a2t(box.min_corner());
    auto max_tup = a2t(box.max_corner());

    std::vector<std::array<size_t, Dim>> temp;
    for (auto it=this->pgm_idx->range(min_tup, max_tup); it!=this->pgm_idx->end(); ++it) {
        temp.emplace_back(get_array_from_tuple(*it));
    }

    auto end = std::chrono::steady_clock::now();
    range_count++;
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    Points result;
    result.reserve(temp.size());
    for (auto& tp : temp) {
        Point p;
        for (size_t d=0; d<Dim; ++d) {
            p[d] = tp[d];
        }
        result.emplace_back(p);
    }
    
    return result;
}

// this is approx knn not exact knn
Points knn_query(Point& q, size_t k) {
    auto start = std::chrono::steady_clock::now();

    auto q_tup = a2t(q);
    auto results = this->pgm_idx->knn(q_tup, k);
    
    auto end = std::chrono::steady_clock::now();
    knn_count++;
    knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::vector<std::array<size_t, Dim>> temp;
    temp.reserve(k);
    Points result_points;
    result_points.reserve(k);

    for (auto& tup : results) {
        temp.emplace_back(get_array_from_tuple(tup));
    }
    for (auto& tp : temp) {
        Point p;
        for (size_t d=0; d<Dim; ++d) {
            p[d] = tp[d];
        }
        result_points.emplace_back(p);
    }
    return result_points;
}

inline size_t count() {
    return _data.size();
}

// index size in Bytes
// pgm index only compute the space cost of line segments
// to make it fair, we add the size of index payloads, i.e., ids (pointers) of each point
inline size_t index_size() {
    return pgm_idx->size_in_bytes() + count() * sizeof(size_t);
}

inline size_t get_resolution() {
    return this->resolution;
}

private:
// the grid resolution to compute the z address
// by default, it is set to N^{1/d}
size_t resolution;

std::array<double, Dim> mins;
std::array<double, Dim> maxs;
std::array<double, Dim> widths;

// internal data
Points& _data;
// internal pgm index
Index* pgm_idx;

// turn a double point to ints to compute the z-value
inline size_t to_id(double val, size_t I) {
    if (val <= this->mins[I]) {
        return 0;
    }
    if (val >= this->maxs[I]) {
        return (this->maxs[I] - this->mins[I]) / this->widths[I];
    }
    return static_cast<size_t>((val - this->mins[I])/this->widths[I]);
}

template<typename Array, std::size_t... I>
inline auto a2t_impl(const Array& a, std::index_sequence<I...>) {
    return std::make_tuple(to_id(a[I], I)...);
}
 
template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
inline auto a2t(const std::array<T, N>& a) {
    return a2t_impl(a, Indices{});
}

template<typename tuple_t>
constexpr auto get_array_from_tuple(tuple_t&& tuple) {
    constexpr auto get_array = [](auto&& ... x){ return std::array{std::forward<decltype(x)>(x) ... }; };
    return std::apply(get_array, std::forward<tuple_t>(tuple));
}

};

}
}
