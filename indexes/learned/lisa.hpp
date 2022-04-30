#pragma once

#include <cstddef>
#include <vector>
#include <chrono>
#include <boost/geometry/index/rtree.hpp>

#include "../pgm/pgm_index.hpp"
#include "../base_index.hpp"
#include "../../utils/type.hpp"
#include "../../utils/common.hpp"


namespace bgi = boost::geometry::index;

namespace bench { namespace index {

template <size_t Dim, size_t BucketSize, size_t Epsilon>
class LISA : public BaseIndex {

using Point = point_t<Dim>;
using Box = box_t<Dim>;
using Points = std::vector<Point>;
using PGMIndex = pgm::PGMIndex<double, Epsilon>;

struct Bucket {
    size_t id;
    double volume;

    Bucket(size_t _id, double _volume) : id(_id), volume(_volume) {}
};

using packing_rtree_t = bgi::rtree<Point, bgi::linear<BucketSize>>;
using grid_rtree_t = bgi::rtree<std::pair<Box, Bucket>, bgi::linear<32>>;


public:
LISA(Points& points) {
    auto start = std::chrono::steady_clock::now();
    
    // compute a grid index layout using STR packing
    std::vector<std::pair<Box, Bucket>> grid_buckets;
    grid_buckets.reserve((points.size() + BucketSize - 1) / BucketSize);

    packing_rtree_t temp_rt(points.begin(), points.end());
    Points temp_pts;
    temp_pts.reserve(BucketSize);
    size_t cnt = 0;
    size_t grid_id = 0;
    for (auto it=temp_rt.begin(); it!=temp_rt.end(); ++it) {
        temp_pts.emplace_back(*it);
        if ((++cnt) % BucketSize == 0) {
            auto mbr = compute_mbr(temp_pts);
            grid_buckets.emplace_back(mbr, Bucket(grid_id++, compute_volume(mbr)));
            temp_pts.clear();
        }
    }

    if (temp_pts.size() != 0) {
        auto mbr = compute_mbr(temp_pts);
        grid_buckets.emplace_back(mbr, Bucket(grid_id++, compute_volume(mbr)));
        temp_pts.clear();
    }

    // build rtree for indexing grid buckets
    _grid_rt = new grid_rtree_t(grid_buckets.begin(), grid_buckets.end());

    // sort data by projections and train CDF model
    std::vector<std::pair<Point, double>> point_with_proj;
    point_with_proj.reserve(points.size());
    for (auto& p : points) {
        point_with_proj.emplace_back(p, project(p));
    }

    std::sort(point_with_proj.begin(), point_with_proj.end(), 
        [](auto p1, auto p2) { return std::get<1>(p1) < std::get<1>(p2); });

    this->_data.reserve(points.size());
    std::vector<double> projections;
    projections.reserve(points.size());

    for (auto& pp : point_with_proj) {
        this->_data.emplace_back(std::get<0>(pp));
        projections.emplace_back(std::get<1>(pp));
    }

    // for (size_t i=0; i<10; ++i) {
    //     auto idx = 5000 * i;
    //     std::cout << "id: " << idx << " val: " << projections[idx] << std::endl;
    //     bench::common::print_point(_data[idx]);
    //     print_bucket_to_point(_data[idx]);
        

    //     std::cout << "id: " << idx+1 << " val: " << projections[idx+1] << std::endl;
    //     bench::common::print_point(_data[idx+1]);
    //     print_bucket_to_point(_data[idx+1]);
    //     std::cout << "===========" <<std::endl;
    // }

    // resolve the duplicate key problem caused by points exactly on the grid boundary
    for (size_t i=0; i<projections.size(); ++i) {
        if (i % BucketSize == 0) {
            for (size_t j=i; j<i+Dim; ++j) {
                projections[j] += (j - i + 1) * 0.00000000001;
            }
        }
    }
    
    for (size_t i=0; i<projections.size()-1; ++i) {
        if (projections[i] == projections[i+1]) {
            std::cout << "duplicate id: " << i << " val: " << projections[i] << std::endl;
        }
    }

    // as we focus on read-only performance, we omit the steps of reordering data by the predictions, 
    // which is essential to make the index updatable
    this->_pgm_idx = new PGMIndex(projections);

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

~LISA() {
    delete this->_grid_rt;
    // delete this->_pgm_idx;
}


Points range_query(Box& box) {
    auto start = std::chrono::steady_clock::now();

    auto range_min = this->_pgm_idx->search(project(box.min_corner()));
    auto range_max = this->_pgm_idx->search(project(box.max_corner()));

    auto it_lo = this->_data.begin() + range_min.lo;
    auto it_hi = this->_data.begin() + range_max.hi;

    Points result;
    for (auto it=it_lo; it!=it_hi; ++it) {
        if (bench::common::is_in_box(*it, box)) {
            result.emplace_back(*it);
        }
    }

    auto end = std::chrono::steady_clock::now();
    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    range_count ++;

    return result;
}

inline size_t count() {
    return this->_data.size();
}


inline size_t index_size() {
    auto grid_size = bench::common::get_boost_rtree_statistics(*(this->_grid_rt));
    auto pgm_size = this->_pgm_idx->size_in_bytes();
    return grid_size + pgm_size + count() * sizeof(size_t);
}

inline void print_bucket_to_point(Point& p) {
    auto q_result = *(this->_grid_rt->qbegin(bgi::intersects(p)));
    std::cout << "bucket id: " << q_result.second.id << " vol: " << q_result.second.volume << std::endl;
    bench::common::print_box(q_result.first);
}

inline void print_buckets() {
    for (auto it=_grid_rt->begin(); it!=_grid_rt->end(); ++it) {
        auto p = *it;
        std::cout << "ID: " << p.second.id << " Volume: " << p.second.volume << std::endl;
        bench::common::print_box(p.first);
    }
}

inline double project(Point& point) {
    auto q_result = *(this->_grid_rt->qbegin(bgi::intersects(point)));
    auto relative_vol = compute_volume(q_result.first, point);
    return static_cast<double>(q_result.second.id) + relative_vol / q_result.second.volume;
}

private:
Points _data;
grid_rtree_t* _grid_rt;
PGMIndex* _pgm_idx;


inline Box compute_mbr(Points& pts) {
    Point mins, maxs;
    std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
    std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

    for (size_t i=0; i<Dim; ++i) {
        for (auto& pt : pts) {
            mins[i] = std::min(pt[i], mins[i]);
            maxs[i] = std::max(pt[i], maxs[i]);
        }
    }

    return Box(mins, maxs);
}

inline double compute_volume(Box& box) {
    auto min_pt = box.min_corner();
    auto max_pt = box.max_corner();

    double vol = 1.0;
    for (size_t i=0; i<Dim; ++i) {
        // scaled by a factor of 100 to avoid numerical problems
        vol *= (100.0 * (max_pt[i] - min_pt[i]));
    }

    return vol;
}

inline double compute_volume(Box& box, Point& pt) {
    auto min_pt = box.min_corner();

    double vol = 1.0;
    for (size_t i=0; i<Dim; ++i) {
        vol *= (100.0 * (pt[i] - min_pt[i]));
    }
    
    return vol;
}


};

}
}
