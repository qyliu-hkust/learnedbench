#pragma once

#include "nanoflann.hpp"
#include "../utils/type.hpp"
#include "../utils/common.hpp"
#include <cstddef>
#include <vector>
#include <chrono>

namespace bench { namespace index {

// kdtree adapter using nanoflann
template <size_t Dim, size_t MaxSplit=10>
class KDTree {

using Point = point_t<Dim>;
using Box = box_t<Dim>;
using Points = std::vector<point_t<Dim>>;


// ===== This example shows how to use nanoflann with these types of containers:
// using my_vector_of_vectors_t = std::vector<std::vector<double> > ;
//
// The next one requires #include <Eigen/Dense>
// using my_vector_of_vectors_t = std::vector<Eigen::VectorXd> ;
// =============================================================================

/** A simple vector-of-vectors adaptor for nanoflann, without duplicating the
 * storage. The i'th vector represents a point in the state space.
 *
 *  \tparam DIM If set to >0, it specifies a compile-time fixed dimensionality
 *      for the points in the data set, allowing more compiler optimizations.
 *  \tparam num_t The type of the point coordinates (typ. double or float).
 *  \tparam Distance The distance metric to use: nanoflann::metric_L1,
 *          nanoflann::metric_L2, nanoflann::metric_L2_Simple, etc.
 *  \tparam IndexType The type for indices in the KD-tree index
 *         (typically, size_t of int)
 */
template <
    class VectorOfVectorsType, typename num_t = double, int DIM = -1,
    class Distance = nanoflann::metric_L2, typename IndexType = size_t>
struct KDTreeVectorOfVectorsAdaptor
{
    using self_t =
        KDTreeVectorOfVectorsAdaptor<VectorOfVectorsType, num_t, DIM, Distance>;
    using metric_t =
        typename Distance::template traits<num_t, self_t>::distance_t;
    using index_t =
        nanoflann::KDTreeSingleIndexAdaptor<metric_t, self_t, DIM, IndexType>;

    /** The kd-tree index for the user to call its methods as usual with any
     * other FLANN index */
    index_t* index = nullptr;

    /// Constructor: takes a const ref to the vector of vectors object with the
    /// data points
    KDTreeVectorOfVectorsAdaptor(
        const size_t /* dimensionality */, const VectorOfVectorsType& mat,
        const int leaf_max_size = 10)
        : m_data(mat)
    {
        assert(mat.size() != 0 && mat[0].size() != 0);
        const size_t dims = mat[0].size();
        if (DIM > 0 && static_cast<int>(dims) != DIM)
            throw std::runtime_error(
                "Data set dimensionality does not match the 'DIM' template "
                "argument");
        index = new index_t(
            static_cast<int>(dims), *this /* adaptor */,
            nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    ~KDTreeVectorOfVectorsAdaptor() { delete index; }

    const VectorOfVectorsType& m_data;

    /** Query for the \a num_closest closest points to a given point
     *  (entered as query_point[0:dim-1]).
     *  Note that this is a short-cut method for index->findNeighbors().
     *  The user can also call index->... methods as desired.
     *
     * \note nChecks_IGNORED is ignored but kept for compatibility with
     * the original FLANN interface.
     */
    inline void query(
        const num_t* query_point, const size_t num_closest,
        IndexType* out_indices, num_t* out_distances_sq) const
    {
        nanoflann::KNNResultSet<num_t, IndexType> resultSet(num_closest);
        resultSet.init(out_indices, out_distances_sq);
        index->findNeighbors(resultSet, query_point, nanoflann::SearchParams());
    }

    /** @name Interface expected by KDTreeSingleIndexAdaptor
     * @{ */

    const self_t& derived() const { return *this; }
    self_t&       derived() { return *this; }

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return m_data.size(); }

    // Returns the dim'th component of the idx'th point in the class:
    inline num_t kdtree_get_pt(const size_t idx, const size_t dim) const
    {
        return m_data[idx][dim];
    }

    // Optional bounding-box computation: return false to default to a standard
    // bbox computation loop.
    // Return true if the BBOX was already computed by the class and returned
    // in "bb" so it can be avoided to redo it again. Look at bb.size() to
    // find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const
    {
        return false;
    }

    /** @} */

};  // end of KDTreeVectorOfVectorsAdaptor


// customized kdtree type
using kdtree_t = KDTreeVectorOfVectorsAdaptor<Points, double, Dim>;

public:
KDTree(Points& points) {
    std::cout << "Construct kd-tree" << std::endl;

    bench::common::dump_mem_usage();
    auto start = std::chrono::steady_clock::now();
    kdtree = new kdtree_t(Dim, points, MaxSplit);
    kdtree->index->buildIndex();
    auto end = std::chrono::steady_clock::now();
    bench::common::dump_mem_usage();
    std::cout << "Construction time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " [ms]" << std::endl;
}

~KDTree() {
    delete kdtree;
}

Points knn_query(Point& q, unsigned int k) {
    const size_t num_of_results = k;
    std::vector<size_t> ret_indexes(k);
    std::vector<double> out_dist_sqr(k);

    kdtree->query(&q[0], num_of_results, &ret_indexes[0], &out_dist_sqr[0]);

    // final result
    Points result;
    result.reserve(num_of_results);
    for (auto idx : ret_indexes) {
        result.emplace_back(kdtree->m_data[idx]);
    }

    return result;
}


inline size_t count() {
    return kdtree->kdtree_get_point_count();
}

private:
// internal kdtree using nanoflann
kdtree_t* kdtree;

};

}}

