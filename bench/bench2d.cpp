#include "../utils/datautils.hpp"
#include "../indexes/nonlearned/nonlearned_index.hpp"
#include "../indexes/learned/learned_index.hpp"
#include "../utils/common.hpp"

#include <cstddef>
#include <string>

using Point = point_t<2>;
using Points = std::vector<Point>;
using Box = box_t<2>;

using RTree = bench::index::RTree<2>;
using RStarTree = bench::index::RStarTree<2>;
using KDTree = bench::index::KDTree<2>;
using ANNKDTree = bench::index::ANNKDTree<2>;
using QDTree = bench::index::QDTree<2>;
using ZM = bench::index::ZMIndex<2, 32>;

struct IndexSet {
    RTree* rtree = nullptr;
    RStarTree* rstartree = nullptr;
    KDTree* kdtree = nullptr;
    ANNKDTree* annkdtree = nullptr;
    QDTree* qdtree = nullptr;
    ZM* zm = nullptr;

    ~IndexSet() {
        delete rtree;
        delete kdtree;
        delete annkdtree;
        delete qdtree;
        delete zm;
    }
};


static void build_index(IndexSet& idx_set, const std::string& idx_name, Points& points) {
    if (idx_name.compare("rtree") == 0) {
        idx_set.rtree = new RTree(points);
        return;
    }

    if (idx_name.compare("rstar") == 0) {
        idx_set.rstartree = new RStarTree(points);
        return;
    }

    if (idx_name.compare("kdtree") == 0) {
        idx_set.kdtree = new KDTree(points);
        return;
    }

    if (idx_name.compare("ann") == 0) {
        idx_set.annkdtree = new ANNKDTree(points);
        return;
    }

    if (idx_name.compare("qdtree") == 0) {
        idx_set.qdtree = new QDTree(points);
        return;
    }

    if (idx_name.compare("zm") == 0) {
        idx_set.zm = new ZM(points);
        return;
    }
}


int main(int argc, char **argv) {
    assert(argc >= 4);

    std::string index = argv[1]; // index name
    std::string fname = argv[2]; // data file name
    size_t N = std::stoi(argv[3]); // dataset size
    std::string mode = argv[4]; // bench mode

    std::cout << "====================================" << std::endl;
    std::cout << "Load data: " << fname << std::endl;

    Points points;
    bench::utils::read_points(points, fname, N);
    IndexSet idx_set;

#ifdef HEAP_PROFILE
    build_index(idx_set, index, points);
    return 0;
#endif

#ifndef HEAP_PROFILE
    build_index(idx_set, index, points);
    if (mode.compare("range") == 0) {
        
        
        return 0;
    }

    if (mode.compare("knn") == 0) {
        return 0;
    }
#endif
}
