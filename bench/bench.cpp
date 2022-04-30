#include "../utils/datautils.hpp"
#include "../utils/common.hpp"

#include "../indexes/nonlearned/nonlearned_index.hpp"
#include "../indexes/learned/learned_index.hpp"

#include "query.hpp"

#include <cstddef>
#include <string>
#include <stdlib.h>

#ifndef BENCH_DIM
#define BENCH_DIM 2
#endif

#ifndef PARTITION_NUM
#define PARTITION_NUM 10
#endif

#ifndef INDEX_ERROR_THRESHOLD 
#define INDEX_ERROR_THRESHOLD 64
#endif

using Point = point_t<BENCH_DIM>;
using Points = std::vector<point_t<BENCH_DIM>>;
using Box = box_t<BENCH_DIM>;

// non-learned indices
using RTree = bench::index::RTree<BENCH_DIM>;
using RStarTree = bench::index::RStarTree<BENCH_DIM>;
using KDTree = bench::index::KDTree<BENCH_DIM>;
using ANNKDTree = bench::index::ANNKDTree<BENCH_DIM>;
using QDTree = bench::index::QDTree<BENCH_DIM>;

// grid indices
using UG = bench::index::UG<BENCH_DIM, PARTITION_NUM>;
using EDG = bench::index::EDG<BENCH_DIM, PARTITION_NUM>;

// linear scan
using FS = bench::index::FullScan<BENCH_DIM>;

// learned indices
// note RSMI needs to be compiled individually due to the dependency of an old version of libtorch
// see CMakeLists.txt for more details 
using ZM = bench::index::ZMIndex<BENCH_DIM, INDEX_ERROR_THRESHOLD>;
using MLI = bench::index::MLIndex<BENCH_DIM, INDEX_ERROR_THRESHOLD>;
using IFI = bench::index::IFIndex<BENCH_DIM>;
using Flood = bench::index::Flood<BENCH_DIM, PARTITION_NUM, INDEX_ERROR_THRESHOLD>;
using Lisa = bench::index::LISA2<BENCH_DIM, PARTITION_NUM, INDEX_ERROR_THRESHOLD>;

struct IndexSet {
    RTree*     rtree;
    RStarTree* rstartree;
    KDTree*    kdtree;
    ANNKDTree* annkdtree;
    QDTree*    qdtree;
    UG*        ug;
    EDG*       edg;
    FS*        fs;
    ZM*        zm;
    MLI*       mli;
    IFI*       ifi;
    Flood*     flood;
    Lisa*      lisa;

    IndexSet() : 
        rtree(nullptr), 
        rstartree(nullptr),
        kdtree(nullptr),
        annkdtree(nullptr),
        qdtree(nullptr),
        ug(nullptr),
        edg(nullptr),
        fs(nullptr),
        zm(nullptr),
        mli(nullptr),
        ifi(nullptr),
        flood(nullptr),
        lisa(nullptr) {}

    ~IndexSet() {
        delete rtree;
        delete kdtree;
        delete annkdtree;
        delete qdtree;
        delete ug;
        delete edg;
        delete fs;
        delete zm;
        delete mli;
        delete ifi;
        delete flood;
        delete lisa;
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

    if (idx_name.compare("ug") == 0) {
        idx_set.ug = new UG(points);
        return;
    }

    if (idx_name.compare("edg") == 0) {
        idx_set.edg = new EDG(points);
        return;
    }

    if (idx_name.compare("fs") == 0) {
        idx_set.fs = new FS(points);
        return;
    }

    if (idx_name.compare("zm") == 0) {
        idx_set.zm = new ZM(points);
        return;
    }

    if (idx_name.compare("mli") == 0) {
        idx_set.mli = new MLI(points);
        return;
    }

    if (idx_name.compare("ifi") == 0) {
        idx_set.ifi = new IFI(points);
        return;
    }

    if (idx_name.compare("flood") == 0) {
        idx_set.flood = new Flood(points);
        return;
    }

    if (idx_name.compare("lisa") == 0) {
        idx_set.lisa = new Lisa(points);
        return;
    }

    std::cout << "index name should be one of [rtree, rstar, kdtree, ann, qdtree, ug, edg, fs, zm, mli, ifi, flood, lisa]" << std::endl;
    exit(0);
}


int main(int argc, char **argv) {
    assert(argc >= 4);

    std::string index = argv[1]; // index name
    std::string fname = argv[2]; // data file name
    size_t N = std::stoi(argv[3]); // dataset size
    std::string mode = argv[4]; // bench mode {"range", "knn", "all"}

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
    
    auto range_queries = bench::query::sample_range_queries(points);
    auto knn_queries = bench::query::sample_knn_queries(points);

    build_index(idx_set, index, points);
    

    if (index.compare("rtree") == 0) {
        assert(idx_set.rtree != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.rtree), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.rtree), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.rtree), range_queries);
            bench::query::batch_knn_queries(*(idx_set.rtree), knn_queries);
            return 0;
        }
    }

    if (index.compare("rstar") == 0) {
        assert(idx_set.rstartree != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.rstartree), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.rstartree), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.rstartree), range_queries);
            bench::query::batch_knn_queries(*(idx_set.rstartree), knn_queries);
            return 0;
        }
    }

    if (index.compare("kdtree") == 0) {
        assert(idx_set.kdtree != nullptr);
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.kdtree), knn_queries);
            return 0;
        }
    }

    if (index.compare("ann") == 0) {
        assert(idx_set.annkdtree != nullptr);
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.annkdtree), knn_queries);
            return 0;
        }
    }

    if (index.compare("qdtree") == 0) {
        assert(idx_set.qdtree != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.qdtree), range_queries);
            return 0;
        }
    }
    
    if (index.compare("ug") == 0) {
        assert(idx_set.ug != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.ug), range_queries);
            return 0;
        }
    }

    if (index.compare("edg") == 0) {
        assert(idx_set.edg != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.edg), range_queries);
            return 0;
        }
    }

    if (index.compare("fs") == 0) {
        assert(idx_set.fs != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.fs), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.fs), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.fs), range_queries);
            bench::query::batch_knn_queries(*(idx_set.fs), knn_queries);
            return 0;
        }
    }
    
    if (index.compare("zm") == 0) {
        assert(idx_set.zm != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.zm), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.zm), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.zm), range_queries);
            bench::query::batch_knn_queries(*(idx_set.zm), knn_queries);
            return 0;
        }
    }

    if (index.compare("mli") == 0) {
        assert(idx_set.mli != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.mli), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.mli), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.mli), range_queries);
            bench::query::batch_knn_queries(*(idx_set.mli), knn_queries);
            return 0;
        }
    }

    if (index.compare("ifi") == 0) {
        assert(idx_set.ifi != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.ifi), range_queries);
            return 0;
        }
    }

    if (index.compare("flood") == 0) {
        assert(idx_set.flood != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.flood), range_queries);
            return 0;
        }
    }

    if (index.compare("lisa") == 0) {
        assert(idx_set.lisa != nullptr);
        if (mode.compare("range") == 0) {
            bench::query::batch_range_queries(*(idx_set.lisa), range_queries);
            return 0;
        }
        if (mode.compare("knn") == 0) {
            bench::query::batch_knn_queries(*(idx_set.lisa), knn_queries);
            return 0;
        }
        if (mode.compare("all") == 0) {
            bench::query::batch_range_queries(*(idx_set.lisa), range_queries);
            bench::query::batch_knn_queries(*(idx_set.lisa), knn_queries);
            return 0;
        }
    }

#endif
}
