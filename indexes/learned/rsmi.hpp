#pragma once

#include <cstddef>
#include <string>
#include <chrono>
#include <queue>

#include "../base_index.hpp"
#include "../rsmi/Constants.h"
#include "../rsmi/RSMI.h"
#include "../rsmi/Point.h"
#include "../rsmi/Mbr.h"
#include "../rsmi/ExpRecorder.h"

#ifdef HEAP_PROFILE
#include <gperftools/heap-profiler.h>
#endif


namespace bench { namespace index {

template<size_t Dim=2>
class RSMIWrapper : public BaseIndex {
static_assert(Dim == 2, "RSMI only supports 2-D data!");

using BenchPoint = point_t<Dim>;
using BenchBox = box_t<Dim>;
using BenchPoints = std::vector<point_t<Dim>>;

public:

RSMIWrapper(BenchPoints& points, const std::string path_to_model) : N(points.size()) {
    std::cout << "Construct RSMI index" << std::endl;

    // type cast for RSMI library
    std::vector<rsmientities::Point> rsmi_points;
    rsmi_points.reserve(this->N);

    for (auto& p : points) {
        rsmi_points.emplace_back(std::get<0>(p), std::get<1>(p));
    }

    auto start = std::chrono::steady_clock::now();

#ifdef HEAP_PROFILE
HeapProfilerStart("rsmi");
#endif

    this->_exp_recorder = new rsmiutils::ExpRecorder();
    this->_rsmi = new RSMI(0, rsmiutils::Constants::MAX_WIDTH);
    this->_rsmi->model_path = path_to_model;
    this->_rsmi->build(*_exp_recorder, rsmi_points);

#ifdef HEAP_PROFILE
HeapProfilerDump("final");
HeapProfilerStop();
#endif

    auto end = std::chrono::steady_clock::now();
    build_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Build Time: " << get_build_time() << " [ms]" << std::endl;
}

~RSMIWrapper() {
    delete this->_rsmi;
    delete this->_exp_recorder;
}

inline size_t count() {
    return N;
}

BenchPoints range_query(BenchBox& box) {
    this->_exp_recorder->window_query_results.clear();
    this->_exp_recorder->window_query_results.shrink_to_fit();

    auto min_corner = box.min_corner();
    auto max_corner = box.max_corner();
    // (x1, y1, x2, y2)
    rsmientities::Mbr mbr(std::get<0>(min_corner), std::get<1>(min_corner), std::get<0>(max_corner), std::get<1>(max_corner));

    auto start = std::chrono::steady_clock::now();
    auto rsmi_result = this->_rsmi->acc_window_query(*_exp_recorder, mbr);
    auto end = std::chrono::steady_clock::now();

    range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    range_count ++;

    BenchPoints results;
    for (auto& r : rsmi_result) {
        BenchPoint temp_p;
        temp_p[0] = r.x;
        temp_p[1] = r.y;
        results.emplace_back(temp_p);
    }

    return results;
}

BenchPoints knn_query(BenchPoint& point, size_t k) {
    // empty the internal queue
    std::priority_queue<rsmientities::Point, std::vector<rsmientities::Point>, rsmiutils::sortForKNN2> temp_pq;
    this->_exp_recorder->pq = temp_pq;

    rsmientities::Point q_point(std::get<0>(point), std::get<1>(point));

    auto start = std::chrono::steady_clock::now();
    std::vector<rsmientities::Point> knn_results = this->_rsmi->acc_kNN_query(*_exp_recorder, q_point, k);
    auto end = std::chrono::steady_clock::now();
    
    knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    knn_count ++;

    BenchPoints results;
    for (auto& r : knn_results) {
        BenchPoint temp_p;
        temp_p[0] = r.x;
        temp_p[1] = r.y;
        results.emplace_back(temp_p);
    }

    return results;
}


private:
size_t N;
RSMI* _rsmi;
ExpRecorder* _exp_recorder;


};

}
}
