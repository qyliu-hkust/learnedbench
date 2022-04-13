#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <variant>
#include <boost/multi_array.hpp>
#include <vector>

#include "type.hpp"
#include "common.hpp"
#include "../pgm/pgm_index.hpp"

namespace bench { namespace index {
template<size_t Dim, size_t SortDim, size_t K, size_t Eps>
class Flood {

static_assert(Dim >= 2, "Expected: Dim >= 2");
static_assert(SortDim >=0 && SortDim < Dim, "SortDim should be in range [0, Dim-1]!");


using Point = point_t<Dim>;
using Points = std::vector<Point>;
using Range = std::pair<size_t, size_t>;
using Box = box_t<Dim>;

using Bucket = std::vector<size_t>;
using Grid = boost::multi_array<Bucket, Dim-1>;

using Index = pgm::PGMIndex<double, Eps>;

public:
Flood(Points& points) : N(points.size()), m_data(points) {
    // sort points by SortDim first
    std::sort(m_data.begin(), m_data.end(), [](auto& p1, auto& p2) {
        return p1[SortDim] < p2[SortDim];
    });

    std::array<size_t, Dim-1> extents;
    std::fill(extents.begin(), extents.end(), K);

    // grid on Dim-1 dimensions (except the SortDim)
    grid = new Grid(extents);
    
    // build index
    for (size_t d=0; d<Dim; ++d) {
        if (d != SortDim) {
            std::vector<double> dim_data(N);
            
            for (auto& p : points) {
                dim_data.emplace_back(p[d]);
            }

            // sort the data
            std::sort(dim_data.begin(), dim_data.end());

            // train pgm index on the sorted 1D data
            auto idx = dim_to_idx(d);
            this->indexes[idx] = new Index(dim_data);
        }
    }

    // insert points to grid based on the learned indexes
    std::array<size_t, Dim-1> idx_array;

    for (size_t i=0; i<N; ++i) {
        for (size_t d=0; d<Dim; ++d) {
            if (d != SortDim) {
                auto idx = dim_to_idx(d);
                auto range = this->indexes[idx]->search(points[i][d]);
                idx_array[idx] = range.pos * K / N;
            }
        }
        this->grid->get(idx_array).emplace_back(i);
    }
}

Points range_query(Box& box) {
    std::array<typename Grid::index, Dim-1> idx_array;
    
}


~Flood() {
    delete grid;
    for (int i=0; i<indexes.size(); ++i) {
        delete indexes[i];
    }
}


private:
size_t N;
Points& m_data;
Grid* grid;
std::array<Index*, Dim-1> indexes;

inline size_t dim_to_idx(size_t d) {
    return d < SortDim ? d : d-1;
}


};
}
}
