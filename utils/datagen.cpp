#include <boost/program_options.hpp>
#include <ios>
#include <iostream>
#include <string>
#include <tpie/tpie.h>
#include "type.hpp"
#include "datautils.hpp"

namespace po = boost::program_options;

int main(int argc, char const *argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("task,t", po::value<std::string>(), "tasks: gen_data, process_csv")
        ("infname", po::value<std::string>(), "input data file name")
        ("fname,f", po::value<std::string>(), "file name of generated data")
        ("dist", po::value<std::string>(), "distribution used to generate data")
        ("num,n", po::value<int>(), "number of points to be generated")
        ("dim,d", po::value<int>(), "dimension of points to be generated")
        ("scale,s", po::value<double>(), "distribution scale factor")
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
        } else {
            std::cout << "Arg --task is in [gen_data, process_csv]." << std::endl;
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
