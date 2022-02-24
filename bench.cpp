#include <boost/program_options.hpp>
#include "utils/datautils.hpp"



namespace po = boost::program_options;

// benchmark R tree
void bench_rtree() {

}

// benchmark quad tree
void bench_quadtree() {

}

// benchmark kd tree
void bench_kdtree() {

}

// benchmark uniform grid
void bench_ug() {

}

// benchmark equal-depth grid
void bench_edg() {

}

// benchmark ZM index
void bench_zmindex() {

}

// benchmark ML index
void bench_mlindex() {

}

// benchmark LISA
void bench_lisa() {

}

// benchmark IF index
void bench_ifindex() {

}

// benchmark RSMI
void bench_rsmi() {

}

// benchmark Flood
void bench_flood() {

}

// benchmark Tsunami
void bench_tsunami() {

}


int main(int argc, char const *argv[]) {
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("task", po::value<std::string>(), "tasks: gen_data, bench_index")
        ("fname,f", po::value<std::string>(), "file name of generated data")
        ("dist", po::value<std::string>(), "distribution used to generate data")
        ("num,n", po::value<int>(), "number of points to be generated")
        ("dim,d", po::value<int>(), "dimension of points to be generated")
        ("scale,s", po::value<double>(), "distribution scale factor")
        ("index,i", po::value<std::string>(), "index name to be benchmarked")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("task")) {
        std::string task = vm["task"].as<std::string>();

        if (task.compare("gen_data") == 0) {
            if (vm.count("fname") && vm.count("dist") && vm.count("num") && vm.count("dim") && vm.count("scale")) {
                std::string fname = vm["fname"].as<std::string>();
                std::string dist = vm["dist"].as<std::string>();
                int n = vm["num"].as<int>();
                int d = vm["dim"].as<int>();
                double s = vm["scale"].as<double>();

                if (dist.compare("uniform") == 0) {
                    // generate uniform data
                    std::cout << "Generate " << n << " * " << d << "D Uniform" << "(0, " << s << ") data to file: " << fname << std::endl;
                    gen_uniform(fname, n, d, s);
                } else if (dist.compare("gaussian") == 0) {
                    // generate gaussian data
                    std::cout << "Generate " << n << " * " << d << "D Gaussian" << "(0, " << s << "^2) data to file: " << fname << std::endl;
                    gen_gaussian(fname, n, d, s);
                } else if (dist.compare("lognormal") == 0) {
                    // generate lognormal data
                    std::cout << "Generate " << n << " * " << d << "D Lognormal" << "(0, " << s << ") data to file: " << fname << std::endl;
                    gen_lognormal(fname, n, d, s);
                } else {
                    std::cout << "Arg --dist is in [uniform, gaussian, lognormal]." << std::endl;
                    return 1;
                }
            } else {
                std::cout << "Please provide fname, dist, num, dim, and scale." << std::endl;
                return 1;
            }
        } else if (task.compare("bench_index") == 0) {
            if (vm.count("index")) {
                std::string index = vm["index"].as<std::string>();
                
                if (index.compare("rtree") == 0) {
                    std::cout << "Benchmark index: R-tree" << std::endl;
                    bench_rtree();
                } else if (index.compare("kdtree") == 0) {
                    std::cout << "Benchmark index: kd-tree" << std::endl;
                    bench_kdtree();
                } else if (index.compare("quadtree") == 0) {
                    std::cout << "Benchmark index: quad-tree" << std::endl;
                    bench_quadtree();
                } else if (index.compare("ug") == 0) {
                    std::cout << "Benchmark index: uniform grid" << std::endl;
                    bench_ug();
                } else if (index.compare("edg") == 0) {
                    std::cout << "Benchmark index: equal-depth grid" << std::endl;
                    bench_edg();
                } else if (index.compare("zm") == 0) {
                    std::cout << "Benchmark index: ZM index" << std::endl;
                    bench_zmindex();
                } else if (index.compare("ml") == 0) {
                    std::cout << "Benchmark index: ML index" << std::endl;
                    bench_mlindex();
                } else if (index.compare("lisa") == 0) {
                    std::cout << "Benchmark index: LISA" << std::endl;
                    bench_lisa();
                } else if (index.compare("if") == 0) {
                    std::cout << "Benchmark index: IF index" << std::endl;
                    bench_ifindex();
                } else if (index.compare("rsmi") == 0) {
                    std::cout << "Benchmark index: RSMI" << std::endl;
                    bench_rsmi();
                } else if (index.compare("flood") == 0) {
                    std::cout << "Benchmark index: Flood" << std::endl;
                    bench_flood();
                } else if (index.compare("tsunami") == 0) {
                    std::cout << "Benchmark index: Tsunami" << std::endl;
                    bench_tsunami();
                } else {
                    std::cout << "Arg --index is in [rtree, kdtree, quadtree, ug, edg, zm, ml, lisa, if, rsmi, flood, tsunami]." << std::endl;
                    return 1;
                }
            } else {
                std::cout << "Please provide index." << std::endl;
                return 1;
            }
        } else {
            std::cout << "Arg --task is in [gen_data, bench_index]." << std::endl;
            return 1;
        }
    } else {
        std::cout << "Specify a task." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    return 0;
}
