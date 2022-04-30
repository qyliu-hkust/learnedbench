#include "../utils/datautils.hpp"
#include "../utils/common.hpp"
#include "../indexes/learned/ifindex.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>




int main() {
    vec_of_point_t<2> points;

    bench::utils::read_points(points, "/home/qiyu/learnedbench/data/synthetic/N/uniform_1m_2_1", 1000000);

    bench::index::IFIndex<2, 100000> ifidx(points);

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
