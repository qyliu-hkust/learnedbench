#pragma once

#include <array>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/io/io.hpp>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <tpie/file_stream.h>
#include <assert.h>
#include <tpie/tpie.h>
#include <utility>
#include <vector>
#include "common.hpp"
#include "type.hpp"


namespace bench { namespace utils { 

// generate n d-dimensional uniform points in range [0, r]
inline void gen_uniform(const std::string& fname, const int n, const int d, const double r) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> dis(0.0, r);

    tpie::file_stream<double> out;
    out.open(fname);
    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }
    
    out.close();
}


// generate n d-dimensional points from a Gaussian distribution N(0, s^2)
inline void gen_gaussian(const std::string& fname, const int n, const int d, const double s) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::normal_distribution<> dis(0.0, s);

    tpie::file_stream<double> out;
    out.open(fname);
    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }
    out.close();
}


// generate n d-dimensional points from a lognormal distribution mean=0 sigma=s
inline void gen_lognormal(const std::string& fname, const int n, const int d, const double s) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::lognormal_distribution<> dis(0.0, s);

    tpie::file_stream<double> out;
    out.open(fname);

    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }

    out.close();
}


struct dist_info_t {
    std::string dist_name;
    int n;
    int d;
};


// read data
inline void read_data(vec_of_double_vec_t& vv, const std::string& fname, const int n, const int d) {
    vv.resize(n);

    // tpie::tpie_init();
    tpie::file_stream<double> in;
    in.open(fname);

    for (int i=0; i<n; ++i) {
        vv[i].resize(d);
        for (int j=0; j<d; ++j) {
            vv[i][j] = in.read();
        }
    }

    in.close();
    // tpie::tpie_finish();
}


template<size_t dim>
inline void read_points(vec_of_point_t<dim>& out_points, const std::string& fname, const size_t N) {
    out_points.reserve(N);

    tpie::tpie_init();
    tpie::file_stream<double> in;
    in.open(fname);

    point_t<dim> p;
    for (size_t i=0; i<N; ++i) {
        for (size_t j=0; j<dim; ++j) {
            p[j] = in.read();
        }
        out_points.emplace_back(p);
    }

    in.close();
    tpie::tpie_finish();
}


template<typename T, typename Iter, std::size_t... Is>
constexpr auto to_array(Iter& iter, std::index_sequence<Is...>)
-> std::array<T, sizeof...(Is)> {
    return {{ ((void)Is, *iter++)... }};
}


template<std::size_t N, typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
constexpr auto to_array(Iter iter)
-> std::array<T, N> {
    return to_array<T>(iter, std::make_index_sequence<N>{});
}


// transform data to vector of points
template<size_t dim>
inline auto to_points(vec_of_double_vec_t& vv) {
    std::vector<std::array<double, dim>> points;
    auto n = vv.size();
    points.reserve(n);

    for (size_t i=0; i<n; ++i) {
        // use move instead of copy to save memory
        points.push_back(to_array<dim>(std::make_move_iterator(vv[i].begin())));
    }

    return points;
}


// parse csv data
inline void csv_to_bin(const std::string& in_fname, const std::string& out_fname) {
    // open tpie for output
    // tpie::tpie_init();
    tpie::file_stream<double> out;
    out.open(out_fname);
    
    // open input csv file
    std::ifstream infile(in_fname);
    std::string line;

    size_t count = 0;
    while (std::getline(infile, line)) {
        line.pop_back();
        std::istringstream is(line);
        std::string val_str;
        while (std::getline(is, val_str, ',')) {
            out.write(std::stod(val_str));
            count++;
        }
    }

    infile.close();
    out.close();
    // tpie::tpie_finish();
}


// trasform csv format data to binary data
inline void csv_to_bin(const std::string& in_fname, const std::string& out_fname, const char delimiter, const int n, const int d) {
    std::ifstream in_fs(in_fname);
    // tpie::tpie_init();
    tpie::file_stream<double> out;
    out.open(out_fname);

    std::string line;
    int count_n = 0;
    while (std::getline(in_fs, line)) {
        std::stringstream line_stream(line);
        std::string cell;
        int count_d = 0;
        while (std::getline(line_stream, cell, delimiter)) {
            out.write(std::stod(cell));
            count_d++;
        }

        // chech dimension
        assert(count_d == d);
        count_n++;
    }

    // check total size
    assert(count_n == n);

    in_fs.close();
    out.close();
    // tpie::tpie_finish();
}

}
}

