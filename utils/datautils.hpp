#pragma once

#include <random>
#include <string>
#include <iostream>
#include <tpie/file_stream.h>
#include <assert.h>
#include "type.hpp"


// generate n d-dimensional uniform points in range [0, r]
void gen_uniform(const std::string& fname, const int n, const int d, const double r) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> dis(0.0, r);

    tpie::tpie_init();
    tpie::file_stream<double> out;
    out.open(fname);
    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }
    
    out.close();
    tpie::tpie_finish();
}


// generate n d-dimensional points from a Gaussian distribution N(0, s^2)
void gen_gaussian(const std::string& fname, const int n, const int d, const double s) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::normal_distribution<> dis(0.0, s);

    tpie::tpie_init();
    tpie::file_stream<double> out;
    out.open(fname);
    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }
    out.close();
    tpie::tpie_finish();
}


// generate n d-dimensional points from a lognormal distribution mean=0 sigma=s
void gen_lognormal(const std::string& fname, const int n, const int d, const double s) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::lognormal_distribution<> dis(0.0, s);

    tpie::tpie_init();
    tpie::file_stream<double> out;
    out.open(fname);
    for (int i=0; i<n*d; ++i) {
        out.write(dis(gen));
    }
    out.close();
    tpie::tpie_finish();
}


struct dist_info_t {
    std::string dist_name;
    int n;
    int d;
};


// read data
void read_data(vec_of_double_vec_t& vv, const std::string& fname, const int n, const int d) {
    vv.reserve(n);

    tpie::tpie_init();
    tpie::file_stream<double> in;
    in.open(fname);
    for (int i=0; i<n; ++i) {
        vv[i].resize(d);
        for (int j=0; j<d; ++j) {
            vv[i][j] = in.read();
        }
    }

    assert(!in.can_read());

    in.close();
    tpie::tpie_finish();
}

