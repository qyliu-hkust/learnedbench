// #pragma once

// #include "../indexes/nonlearned/nonlearned_index.hpp"
// #include "../indexes/learned/learned_index.hpp"

// #include "../utils/common.hpp"
// #include "../utils/datautils.hpp"
// #include "../utils/type.hpp"

// #include <array>
// #include <cassert>
// #include <chrono>
// #include <cmath>
// #include <cstddef>
// #include <cstdio>
// #include <iostream>
// #include <sstream>
// #include <unistd.h>


// namespace bench { namespace nonlearnedbench {

// struct FSBench : bench::common::BaseBench {
//     template<class Index, size_t Dim>
//     static void prepare_and_run(Index& index, vec_of_point_t<Dim>& points) {
//         auto range_queries = sample_range_queries(points);
//         auto knn_queries = sample_knn_queries(points);

//         batch_knn_queries(index, knn_queries);
//         batch_range_queries(index, range_queries);
//     }
    
//     static void bench_index_default() {
//         // bench synthetic
//         {
//             std::array<std::string, 3> dists = {"uniform", "gaussian", "lognormal"};
//             for (const auto& dist : dists) {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::DEFAULT);
//                 bench::common::FullScan<2> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//         // bench fs
//         {
//             vec_of_point_t<2> points;
//             load_fs(points);
//             bench::common::FullScan<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench toronto
//         {
//             vec_of_point_t<3> points;
//             load_toronto(points);
//             bench::common::FullScan<3> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench osm
//         {
//             vec_of_point_t<2> points;
//             load_osm(points);
//             bench::common::FullScan<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench tpch
//         {
//             vec_of_point_t<4> points;
//             load_tpch(points);
//             bench::common::FullScan<4> index(points);
//             prepare_and_run(index, points);
//         }

//     }

//     static void bench_index_N() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};
//         std::array<size_t, 4> N_array = {1, 10, 50, 100};

//         for (const auto& dist : dist_array) {
//             for (const auto& N : N_array) {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, N, flag::VARY_N);
//                 bench::common::FullScan<3> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void bench_index_Dim() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};

//         for (const auto& dist : dist_array) {
//             // dim=2
//             {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::common::FullScan<2> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=3
//             {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::common::FullScan<3> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=4
//             {
//                 vec_of_point_t<4> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::common::FullScan<4> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=5
//             {
//                 vec_of_point_t<5> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::common::FullScan<5> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=10
//             {
//                 vec_of_point_t<10> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::common::FullScan<10> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void run(std::string& mode) {
//         if (mode.compare("default") == 0) {
//             bench_index_default();
//         } else if (mode.compare("vary_n") == 0) {
//             bench_index_N();
//         } else if (mode.compare("vary_d") == 0) {
//             bench_index_Dim();
//         } else {
//             return;
//         }
//     }
// };

// struct RTreeBench : bench::common::BaseBench {

//     template<class Index, size_t Dim>
//     static void prepare_and_run(Index& index, vec_of_point_t<Dim>& points) {
//             auto range_queries = sample_range_queries(points);
//             auto knn_queries = sample_knn_queries(points);

//             batch_knn_queries(index, knn_queries);
//             batch_range_queries(index, range_queries);
//     }

//     static void bench_index_default() {
//         // bench synthetic
//         {
//             std::array<std::string, 3> dists = {"uniform", "gaussian", "lognormal"};
//             for (const auto& dist : dists) {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::DEFAULT);
//                 bench::index::RTree<2> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//         // bench fs
//         {
//             vec_of_point_t<2> points;
//             load_fs(points);
//             bench::index::RTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench toronto
//         {
//             vec_of_point_t<3> points;
//             load_toronto(points);
//             bench::index::RTree<3> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench osm
//         {
//             vec_of_point_t<2> points;
//             load_osm(points);
//             bench::index::RTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench tpch
//         {
//             vec_of_point_t<4> points;
//             load_tpch(points);
//             bench::index::RTree<4> index(points);
//             prepare_and_run(index, points);
//         }

//     }

//     static void bench_index_N() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};
//         std::array<size_t, 4> N_array = {1, 10, 50, 100};

//         for (const auto& dist : dist_array) {
//             for (const auto& N : N_array) {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, N, flag::VARY_N);
//                 bench::index::RTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void bench_index_Dim() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};

//         for (const auto& dist : dist_array) {
//             // dim=2
//             {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::RTree<2> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=3
//             {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::RTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=4
//             {
//                 vec_of_point_t<4> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::RTree<4> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=5
//             {
//                 vec_of_point_t<5> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::RTree<5> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=10
//             {
//                 vec_of_point_t<10> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::RTree<10> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void run(std::string& mode) {
//         if (mode.compare("default") == 0) {
//             bench_index_default();
//         } else if (mode.compare("vary_n") == 0) {
//             bench_index_N();
//         } else if (mode.compare("vary_d") == 0) {
//             bench_index_Dim();
//         } else {
//             return;
//         }
//     }
// };


// struct KDTreeBench : bench::common::BaseBench {
//     template<class Index, size_t Dim>
//     static void prepare_and_run(Index& index, vec_of_point_t<Dim>& points) {
//             auto knn_queries = sample_knn_queries(points);
//             batch_knn_queries(index, knn_queries);
//     }

//     static void bench_index_default() {
//         // bench synthetic
//         {
//             std::array<std::string, 3> dists = {"uniform", "gaussian", "lognormal"};
//             for (const auto& dist : dists) {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::DEFAULT);
//                 bench::index::KDTree<2> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//         // bench fs
//         {
//             vec_of_point_t<2> points;
//             load_fs(points);
//             bench::index::KDTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench toronto
//         {
//             vec_of_point_t<3> points;
//             load_toronto(points);
//             bench::index::KDTree<3> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench osm
//         {
//             vec_of_point_t<2> points;
//             load_osm(points);
//             bench::index::KDTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench tpch
//         {
//             vec_of_point_t<4> points;
//             load_tpch(points);
//             bench::index::KDTree<4> index(points);
//             prepare_and_run(index, points);
//         }
//     }

//     static void bench_index_N() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};
//         std::array<size_t, 4> N_array = {1, 10, 50, 100};

//         for (const auto& dist : dist_array) {
//             for (const auto& N : N_array) {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, N, flag::VARY_N);
//                 bench::index::KDTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void bench_index_Dim() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};

//         for (const auto& dist : dist_array) {
//             // dim=2
//             {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::KDTree<2> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=3
//             {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::KDTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=4
//             {
//                 vec_of_point_t<4> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::KDTree<4> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=5
//             {
//                 vec_of_point_t<5> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::KDTree<5> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=10
//             {
//                 vec_of_point_t<10> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::KDTree<10> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void run(std::string& mode) {
//         if (mode.compare("default") == 0) {
//             bench_index_default();
//         } else if (mode.compare("vary_n") == 0) {
//             bench_index_N();
//         } else if (mode.compare("vary_d") == 0) {
//             bench_index_Dim();
//         } else {
//             return;
//         }
//     }
// };


// struct ANNBench : bench::common::BaseBench {
//     template<class Index, size_t Dim>
//     static void prepare_and_run(Index& index, vec_of_point_t<Dim>& points) {
//         auto knn_queries = sample_knn_queries(points);
//         batch_knn_queries(index, knn_queries);
//     }

//     static void bench_index_default() {
//         // bench synthetic
//         // {
//         //     std::array<std::string, 3> dists = {"uniform", "gaussian", "lognormal"};
//         //     for (const auto& dist : dists) {
//         //         vec_of_point_t<2> points;
//         //         load_synthetic(points, dist, 10, flag::DEFAULT);
//         //         bench::index::ANNBoxDecompositionTree<2> index(points);
//         //         prepare_and_run(index, points);
//         //     }
//         // }
//         // bench fs
//         {
//             vec_of_point_t<2> points;
//             load_fs(points);
//             bench::index::ANNBoxDecompositionTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench toronto
//         {
//             vec_of_point_t<3> points;
//             load_toronto(points);
//             bench::index::ANNBoxDecompositionTree<3> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench osm
//         {
//             vec_of_point_t<2> points;
//             load_osm(points);
//             bench::index::ANNBoxDecompositionTree<2> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench tpch
//         {
//             vec_of_point_t<4> points;
//             load_tpch(points);
//             bench::index::ANNBoxDecompositionTree<4> index(points);
//             prepare_and_run(index, points);
//         }
//     }

//     static void bench_index_N() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};
//         std::array<size_t, 4> N_array = {1, 10, 50, 100};

//         for (const auto& dist : dist_array) {
//             for (const auto& N : N_array) {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, N, flag::VARY_N);
//                 bench::index::ANNBoxDecompositionTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void bench_index_Dim() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};

//         for (const auto& dist : dist_array) {
//             // dim=2
//             {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::ANNBoxDecompositionTree<2> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=3
//             {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::ANNBoxDecompositionTree<3> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=4
//             {
//                 vec_of_point_t<4> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::ANNBoxDecompositionTree<4> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=5
//             {
//                 vec_of_point_t<5> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::ANNBoxDecompositionTree<5> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=10
//             {
//                 vec_of_point_t<10> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::ANNBoxDecompositionTree<10> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void run(std::string& mode) {
//         if (mode.compare("default") == 0) {
//             bench_index_default();
//         } else if (mode.compare("vary_n") == 0) {
//             bench_index_N();
//         } else if (mode.compare("vary_d") == 0) {
//             bench_index_Dim();
//         } else {
//             return;
//         }
//     }
// };


// struct UGBench : bench::common::BaseBench {
//     template<class Index, size_t Dim>
//     static void prepare_and_run(Index& index, vec_of_point_t<Dim>& points) {
//         auto range_queries = sample_range_queries(points);

//         batch_range_queries(index, range_queries);
//     }

//     static void bench_index_default() {
//         // bench synthetic
//         {
//             std::array<std::string, 3> dists = {"uniform", "gaussian", "lognormal"};
//             for (const auto& dist : dists) {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::DEFAULT);
//                 bench::index::UG<2, 56> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//         // bench fs
//         {
//             vec_of_point_t<2> points;
//             load_fs(points);
//             bench::index::UG<2, 56> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench toronto
//         {
//             vec_of_point_t<3> points;
//             load_toronto(points);
//             bench::index::UG<3, 56> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench osm
//         {
//             vec_of_point_t<2> points;
//             load_osm(points);
//             bench::index::UG<2, 56> index(points);
//             prepare_and_run(index, points);
//         }
//         // bench tpch
//         {
//             vec_of_point_t<4> points;
//             load_tpch(points);
//             bench::index::UG<4, 16> index(points);
//             prepare_and_run(index, points);
//         }
//     }

//     static void bench_index_N() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};
//         std::array<size_t, 4> N_array = {1, 10, 50, 100};

//         for (const auto& dist : dist_array) {
//             for (const auto& N : N_array) {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, N, flag::VARY_N);
//                 bench::index::UG<3, 32> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void bench_index_Dim() {
//         std::array<std::string, 3> dist_array = {"uniform", "gaussian", "lognormal"};

//         for (const auto& dist : dist_array) {
//             // dim=2
//             {
//                 vec_of_point_t<2> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::UG<2, 56> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=3
//             {
//                 vec_of_point_t<3> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::UG<3, 16> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=4
//             {
//                 vec_of_point_t<4> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::UG<4, 12> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=5
//             {
//                 vec_of_point_t<5> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::UG<5, 8> index(points);
//                 prepare_and_run(index, points);
//             }
//             // dim=10
//             {
//                 vec_of_point_t<10> points;
//                 load_synthetic(points, dist, 10, flag::VARY_DIM);
//                 bench::index::UG<10, 4> index(points);
//                 prepare_and_run(index, points);
//             }
//         }
//     }

//     static void run(std::string& mode) {
//         if (mode.compare("default") == 0) {
//             bench_index_default();
//         } else if (mode.compare("vary_n") == 0) {
//             bench_index_N();
//         } else if (mode.compare("vary_d") == 0) {
//             bench_index_Dim();
//         } else {
//             return;
//         }
//     }
// };


// struct EDGBench {
//     static void run() {

//     }
// };


// }}
