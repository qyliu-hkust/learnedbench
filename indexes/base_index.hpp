#pragma once

#include <cstddef>

// the base index class with basic timing utilities
// note all the timers exclude the cost caused by type casting and result collection
class BaseIndex {
public:
BaseIndex() {
    build_time = 0;
    point_time = 0;
    range_time = 0;
    knn_time = 0;
    point_count = 0;
    range_count = 0;
    knn_count = 0;
}

// return the index construction time
inline size_t get_build_time() {
    return build_time;
}

// return the total time of all historically invoked point queries
inline size_t get_point_time() {
    return point_time;
}

// return the total time of all historically invoked range queries
inline size_t get_range_time() {
    return range_time;
}

// return the total time of all historically invoked knn queries
inline size_t get_knn_time() {
    return knn_time;
}

inline double get_avg_point_time() {
    return (point_time * 1.0) / point_count;
}

inline double get_avg_range_time() {
    return (range_time * 1.0) / range_count;
}

inline double get_avg_knn_time() {
    return (knn_time * 1.0) / knn_count;
}

// reset query timers
// no need to reset build_time
inline void reset_timer() {
    point_time = 0;
    range_time = 0;
    knn_time = 0;
    point_count = 0;
    range_count = 0;
    knn_count = 0;
}

protected:
// unit [ms]
size_t build_time;
// unit [us]
size_t point_time;
// unit [us]
size_t range_time;
// unit [us]
size_t knn_time;

size_t point_count;
size_t range_count;
size_t knn_count;

};

