#include "../utils/datautils.hpp"
#include "../utils/common.hpp"

#include "query.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>

#include "../indexes/nonlearned/geos.hpp"
#include "../indexes/nonlearned/fullscan.hpp"

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;


std::string get_filename(const std::string& path) {
    auto idx = path.find_last_of('/') + 1;
    return path.substr(idx);
}

int main() {
    vec_of_point_t<2> points;

    std::cout << get_filename("/home/qiyu/learnedbench/data/synthetic/uniform_20m_2_1") << std::endl;
    
    bench::utils::read_points(points, "/home/qiyu/learnedbench/data/synthetic/uniform_20m_2_1", 20000000);
    
    // bench::index::RTree<2, 64> rt(points);
    // bench::query::batch_range_queries(rt, range_queries);

    
    point_t<2> min_c = {0.0, 0.0};
    point_t<2> max_c = {0.1, 0.1};
    box_t<2> qb(min_c, max_c);

    bench::index::QDTree<2> qdt(points);
    std::cout << qdt.range_query(qb).size() << std::endl;


    // bench::index::LISA2<4, 10, 64> lisa2(points);

    
    
    
    // std::cout << rt.range_query(q).size() << std::endl;
    // std::cout << "build time " << rt.get_build_time() << std::endl;
    // std::cout << "range query time " << rt.get_range_time() << std::endl;
    // std::cout << "index size " << rt.index_size() << std::endl;

    // bench::index::MLIndex<4, 64, 10> mli(points);
    // std::cout << mli.range_query(q).size() << std::endl;
    // std::cout << "build time " << mli.get_build_time() << std::endl;
    // std::cout << "range query time " << mli.get_range_time() << std::endl;
    // std::cout << "index size " << mli.index_size() << std::endl;

    
    // bench::index::Flood<4, 10, 64> flood(points);
    // std::cout << flood.range_query(q).size() << std::endl;
    // std::cout << "build time " << flood.get_build_time() << std::endl;
    // std::cout << "range query time " << flood.get_range_time() << std::endl;
    // std::cout << "index size " << flood.index_size() << std::endl;

    // bench::index::LISA2<4, 10> lisa(points);
    // std::cout << lisa.range_query(q).size() << std::endl;
    // lisa.knn_query(test_pt, 10000);
    // std::cout << "build time" << lisa.get_build_time() << std::endl;
    // std::cout << "query time" << lisa.get_range_time() << std::endl;
    // std::cout << "knn query time " << lisa.get_knn_time() << std::endl;
    // std::cout << "index size" << lisa.index_size() << std::endl;
    

    // bench::index::IFIndex<2, 2048, 64> ifidx(points);
    // std::cout << ifidx.range_query(q).size() << std::endl;
    // std::cout << "build time" << ifidx.get_build_time() << std::endl;
    // std::cout << "query time" << ifidx.get_range_time() << std::endl;
    // std::cout << "index size" << ifidx.index_size() << std::endl;

    // auto rt = ifidx.get_rtree_ptr();
    // for (auto it=rt->begin(); it!=rt->end(); ++it) {
    //     auto pair = *it;
    //     bench::common::print_box(std::get<0>(pair));
    //     std::get<1>(pair).print_model();
    // }

    // std::vector<size_t> ids;
    // ids.reserve(points.size());

    // for (size_t i=0; i<points.size(); ++i) {
    //     ids.emplace_back(i);
    // }

    // box_t<2> mbr;

    // LeafNode node(mbr, ids, points);

    // node.print_model();
    // std::cout << "err: " << node.max_err << std::endl;


    return 0;
}
