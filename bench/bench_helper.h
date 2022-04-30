#pragma once

#include "../indexes/nonlearned/nonlearned_index.hpp"
#include "../indexes/learned/learned_index.hpp"
#include "../utils/datautils.hpp"

using namespace bench::index;


#define FS_N 3680126
#define TORONTO_N 21567172
#define OSM_N 62734869
#define TPC_N 41995307


// struct IndexSet {
//     bench::index::RTree<2>* rtree_2d = nullptr;
//     bench::index::RTree<3>* rtree_3d = nullptr;
//     bench::index::RTree<4>* rtree_4d = nullptr;
//     bench::index::RTree<5>* rtree_5d = nullptr;
//     bench::index::RTree<10>* rtree_10d = nullptr;

//     ~IndexSet() {
//         delete rtree_2d;
//         delete rtree_3d;
//         delete rtree_4d;
//         delete rtree_5d;
//         delete rtree_10d;
//     }
// };








// static void build_index_3d(IndexSet& idx_set, const std::string& idx_name, vec_of_point_t<3>& points) {
//     if (idx_name.compare("rtree") == 0) {
//         idx_set.rtree_3d = new RTree<3>(points);
//         return;
//     }
// }


// static void build_index_4d(IndexSet& idx_set, const std::string& idx_name, vec_of_point_t<4>& points) {
//     if (idx_name.compare("rtree") == 0) {
//         idx_set.rtree_4d = new RTree<4>(points);
//         return;
//     }
// }


// static void build_index_5d(IndexSet& idx_set, const std::string& idx_name, vec_of_point_t<5>& points) {
//     if (idx_name.compare("rtree") == 0) {
//         idx_set.rtree_5d = new RTree<5>(points);
//         return;
//     }
// }


// static void build_index_10d(IndexSet& idx_set, const std::string& idx_name, vec_of_point_t<10>& points) {
//     if (idx_name.compare("rtree") == 0) {
//         idx_set.rtree_10d = new RTree<10>(points);
//         return;
//     }
// }
