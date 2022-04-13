#include <boost/program_options.hpp>
#include <ios>
#include <iostream>
#include <string>
#include <tpie/tpie.h>
#include "bench.hpp"
#include "common.hpp"
#include "indexes/nonlearned/rtree.hpp"
#include "type.hpp"
#include "utils/datautils.hpp"


namespace po = boost::program_options;

// linear scan
void bench_fs(std::string& mode) {
    bench::nonlearnedbench::FSBench::run(mode);
}

// benchmark R tree
void bench_rtree(std::string& mode) {
    bench::nonlearnedbench::RTreeBench::run(mode);
}

// benchmark ANN
void bench_ann(std::string& mode) {
    bench::nonlearnedbench::ANNBench::run(mode);
}

// benchmark kd tree
void bench_kdtree(std::string& mode) {
    bench::nonlearnedbench::KDTreeBench::run(mode);
}

// benchmark uniform grid
void bench_ug(std::string& mode) {
    bench::nonlearnedbench::UGBench::run(mode);
}

// benchmark equal-depth grid
void bench_edg(std::string& mode) {
    
}

// benchmark ZM index
void bench_zmindex(std::string& mode) {
    
}

// benchmark ML index
void bench_mlindex(std::string& mode) {

}

// benchmark LISA
void bench_lisa(std::string& mode) {

}

// benchmark IF index
void bench_ifindex(std::string& mode) {

}

// benchmark RSMI
void bench_rsmi(std::string& mode) {

}

// benchmark Flood
void bench_flood(std::string& mode) {
    // vec_of_point_t<2> points;
    // // bench::common::BaseBench::load_synthetic(points, "gaussian", 10, bench::common::BaseBench::DEFAULT);
    // bench::common::BaseBench::load_fs(points);

    // auto queries = bench::utils::sample_range_queries(points, 10);
    
    // for (auto& q : queries) {
    //     bench::common::print_box(q.first);
    //     std::cout << "selectivity: " << q.second << std::endl;
    //     auto count =  bench::common::FullScan<2>::range_query(points, q.first).size();
    //     std::cout << "true selectivity: " << (1.0 * count) / points.size() << std::endl;
    // }

    vec_of_point_t<2> points_fs;
    bench::common::BaseBench::load_fs(points_fs);
    std::cout << points_fs.size() << std::endl;

    for (int i=0; i<10; ++i) {
        bench::common::print_point(points_fs[i]);
    }

    vec_of_point_t<3> points_toronto;
    bench::common::BaseBench::load_toronto(points_toronto);
    std::cout << points_toronto.size() << std::endl;
    for (int i=0; i<10; ++i) {
        bench::common::print_point(points_toronto[i]);
    }
}

// benchmark Tsunami
// todo
void bench_tsunami(std::string& mode) {

}


int main(int argc, char const *argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("task,t", po::value<std::string>(), "tasks: gen_data, process_csv, bench_index")
        ("infname", po::value<std::string>(), "input data file name")
        ("fname,f", po::value<std::string>(), "file name of generated data")
        ("dist", po::value<std::string>(), "distribution used to generate data")
        ("num,n", po::value<int>(), "number of points to be generated")
        ("dim,d", po::value<int>(), "dimension of points to be generated")
        ("scale,s", po::value<double>(), "distribution scale factor")
        ("index,i", po::value<std::string>(), "index name to be benchmarked")
        ("mode,m", po::value<std::string>(), "index benchmark mode: default, vary_n, vary_d")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    tpie::tpie_init();

    if (vm.count("task")) {
        std::string task = vm["task"].as<std::string>();

        if (task.compare("process_csv") == 0) {
            if (vm.count("infname") && vm.count("fname")) {
                std::string infname = vm["infname"].as<std::string>();
                std::string outfname = vm["fname"].as<std::string>();

                bench::utils::csv_to_bin(infname, outfname);
            } else {
                std::cout << "Please provide infname and fname." << std::endl;
                tpie::tpie_finish();
                return 1;
            }
        } else if (task.compare("gen_data") == 0) {
            if (vm.count("fname") && vm.count("dist") && vm.count("num") && vm.count("dim") && vm.count("scale")) {
                std::string fname = vm["fname"].as<std::string>();
                std::string dist = vm["dist"].as<std::string>();
                int n = vm["num"].as<int>();
                int d = vm["dim"].as<int>();
                double s = vm["scale"].as<double>();

                if (dist.compare("uniform") == 0) {
                    // generate uniform data
                    std::cout << "Generate " << n << " * " << d << "D Uniform" << "(0, " << s << ") data to file: " << fname << std::endl;
                    bench::utils::gen_uniform(fname, n, d, s);
                } else if (dist.compare("gaussian") == 0) {
                    // generate gaussian data
                    std::cout << "Generate " << n << " * " << d << "D Gaussian" << "(0, " << s << "^2) data to file: " << fname << std::endl;
                    bench::utils::gen_gaussian(fname, n, d, s);
                } else if (dist.compare("lognormal") == 0) {
                    // generate lognormal data
                    std::cout << "Generate " << n << " * " << d << "D Lognormal" << "(0, " << s << ") data to file: " << fname << std::endl;
                    bench::utils::gen_lognormal(fname, n, d, s);
                } else {
                    std::cout << "Arg --dist is in [uniform, gaussian, lognormal]." << std::endl;
                    tpie::tpie_finish();
                    return 1;
                }
            } else {
                std::cout << "Please provide fname, dist, num, dim, and scale." << std::endl;
                tpie::tpie_finish();
                return 1;
            }
        } else if (task.compare("bench_index") == 0) {
            if (vm.count("index") && vm.count("mode")) {
                std::string index = vm["index"].as<std::string>();
                std::string mode = vm["mode"].as<std::string>();
                
                if (index.compare("fs") == 0) {
                    std::cout << "Benchmark index: Full Scan" << std::endl;
                    bench_fs(mode);
                } else if (index.compare("rtree") == 0) {
                    std::cout << "Benchmark index: R-tree" << std::endl;
                    bench_rtree(mode);
                } else if (index.compare("kdtree") == 0) {
                    std::cout << "Benchmark index: kd-tree" << std::endl;
                    bench_kdtree(mode);
                } else if (index.compare("ann") == 0) {
                    std::cout << "Benchmark index: ANN" << std::endl;
                    bench_ann(mode);
                } else if (index.compare("ug") == 0) {
                    std::cout << "Benchmark index: uniform grid" << std::endl;
                    bench_ug(mode);
                } else if (index.compare("edg") == 0) {
                    std::cout << "Benchmark index: equal-depth grid" << std::endl;
                    bench_edg(mode);
                } else if (index.compare("zm") == 0) {
                    std::cout << "Benchmark index: ZM index" << std::endl;
                    bench_zmindex(mode);
                } else if (index.compare("ml") == 0) {
                    std::cout << "Benchmark index: ML index" << std::endl;
                    bench_mlindex(mode);
                } else if (index.compare("lisa") == 0) {
                    std::cout << "Benchmark index: LISA" << std::endl;
                    bench_lisa(mode);
                } else if (index.compare("if") == 0) {
                    std::cout << "Benchmark index: IF index" << std::endl;
                    bench_ifindex(mode);
                } else if (index.compare("rsmi") == 0) {
                    std::cout << "Benchmark index: RSMI" << std::endl;
                    bench_rsmi(mode);
                } else if (index.compare("flood") == 0) {
                    std::cout << "Benchmark index: Flood" << std::endl;
                    bench_flood(mode);
                } else if (index.compare("tsunami") == 0) {
                    std::cout << "Benchmark index: Tsunami" << std::endl;
                    bench_tsunami(mode);
                } else {
                    std::cout << "Arg --index is in [fs, rtree, kdtree, ann, ug, edg, zm, ml, lisa, if, rsmi, flood, tsunami]." << std::endl;
                    tpie::tpie_finish();
                    return 1;
                }
            } else {
                std::cout << "Please provide index and mode." << std::endl;
                tpie::tpie_finish();
                return 1;
            }
        } else {
            std::cout << "Arg --task is in [gen_data, process_csv, bench_index]." << std::endl;
            tpie::tpie_finish();
            return 1;
        }
    } else {
        std::cout << "Specify a task." << std::endl;
        std::cout << desc << std::endl;
        tpie::tpie_finish();
        return 1;
    }

    tpie::tpie_finish();

    return 0;
}
