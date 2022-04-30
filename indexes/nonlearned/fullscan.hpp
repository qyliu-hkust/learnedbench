#pragma once

#include <cstddef>
#include <chrono>
#include "../../utils/type.hpp"
#include "../base_index.hpp"


// a naive baseline FullScan
template<size_t dim>
struct FullScan : public BaseIndex {
    using Point = point_t<dim>;
    using Box = box_t<dim>;
    using Points = std::vector<point_t<dim>>;

    Points& _data;

    FullScan(Points& points) : _data(points) {
        std::cout << "Full Scan Baseline" << std::endl;
    }

    inline size_t count() {
        return _data.size();
    }

    // linear scan O(N)
    Points range_query(Box& box) {
        auto start = std::chrono::steady_clock::now();
        Points result;
        for (auto p : _data) {
            if (is_in_box(p, box)) {
                result.emplace_back(p);
            }
        }
        auto end = std::chrono::steady_clock::now();
        range_count++;
        range_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return result;
    }
    

    // linear scan using a priority queue O(N log K)
    Points knn_query(Point& q, size_t k) {
        using QueueElement = std::pair<Point, double>;
        auto cmp = [&q](QueueElement& e1, QueueElement& e2) { return eu_dist_square(e1.first, q) < e2.second; };
        std::priority_queue<QueueElement, std::vector<QueueElement>, decltype(cmp)> queue(cmp);

        auto start = std::chrono::steady_clock::now(); 
        for (auto p : _data) {
            if (queue.size() < k) {
                queue.push(std::make_pair(p, eu_dist_square(p, q)));
            } else {
                QueueElement const& top_element = queue.top();
                double new_dist = eu_dist_square(p, q);
                if (new_dist < top_element.second) {
                    queue.pop();
                    queue.push(std::make_pair(p, new_dist));
                }
            }
        }
        auto end = std::chrono::steady_clock::now();
        knn_count++;
        knn_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        Points result;
        result.reserve(k);
        while(!queue.empty()) {
            QueueElement const& top_element = queue.top();
            result.emplace_back(top_element.first);
            queue.pop();
        }

        return result;
    }
}; 
