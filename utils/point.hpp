#pragma once

// points definition (2d ~ 15d)

struct point2d_t {
    static int dim;
    float x_1, x_2;
};

struct point3d_t {
    static int dim;
    float x_1, x_2, x_3;
};

struct point4d_t {
    static int dim;
    float x_1, x_2, x_3, x_4;
};

struct point5d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5;
};

struct point6d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6;
};

struct point7d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7;
};

struct point8d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8;
};

struct point9d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9;
};

struct point10d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10;
};

struct point11d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10, x_11;
};

struct point12d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10, x_11, x_12;
};

struct point13d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10, x_11, x_12, x_13;
};

struct point14d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10, x_11, x_12, x_13, x_14;
};

struct point15d_t {
    static int dim;
    float x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9, x_10, x_11, x_12, x_13, x_14, x_15;
};


int point2d_t::dim = 2;
int point3d_t::dim = 3;
int point4d_t::dim = 4;
int point5d_t::dim = 5;
int point6d_t::dim = 6;
int point7d_t::dim = 7;
int point8d_t::dim = 8;
int point9d_t::dim = 9;
int point10d_t::dim = 10;
int point11d_t::dim = 11;
int point12d_t::dim = 12;
int point13d_t::dim = 13;
int point14d_t::dim = 14;
int point15d_t::dim = 15;

