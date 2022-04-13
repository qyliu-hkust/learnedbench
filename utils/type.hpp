#pragma once

#include <array>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/adapted/std_array.hpp>
#include <cstddef>
#include <utility>
#include <vector>
#include <boost/geometry.hpp>


BOOST_GEOMETRY_REGISTER_STD_ARRAY_CS(cs::cartesian)


typedef std::vector<std::vector<double>> vec_of_double_vec_t;
typedef std::vector<std::vector<float>> vec_of_float_vec_t;


namespace bg = boost::geometry;
namespace bgm = bg::model;


// use static point definition
// the dimension should be determined in compile time for performance consideration
// currently, the max dim is 12 in this benchmark
template<size_t dim>
using point_t = std::array<double, dim>;

template<size_t dim>
using pointf_t = std::array<float, dim>;

template<size_t dim>
using box_t = bgm::box<point_t<dim>>;

template<size_t dim>
using boxf_t = bgm::box<pointf_t<dim>>;

template<size_t dim>
using vec_of_point_t = std::vector<point_t<dim>>;

template<size_t dim>
using vec_of_pointf_t = std::vector<pointf_t<dim>>;

template<size_t dim>
using knn_t = std::pair<point_t<dim>, size_t>;

template<size_t dim>
using knnf_t = std::pair<point_t<dim>, size_t>;


typedef point_t<2> point2_t;
typedef point_t<3> point3_t;
typedef point_t<4> point4_t;
typedef point_t<5> point5_t;
typedef point_t<6> point6_t;
typedef point_t<7> point7_t;
typedef point_t<8> point8_t;
typedef point_t<9> point9_t;
typedef point_t<10> point10_t;
typedef point_t<11> point11_t;
typedef point_t<12> point12_t;


typedef pointf_t<2> pointf2_t;
typedef pointf_t<3> pointf3_t;
typedef pointf_t<4> pointf4_t;
typedef pointf_t<5> pointf5_t;
typedef pointf_t<6> pointf6_t;
typedef pointf_t<7> pointf7_t;
typedef pointf_t<8> pointf8_t;
typedef pointf_t<9> pointf9_t;
typedef pointf_t<10> pointf10_t;
typedef pointf_t<11> pointf11_t;
typedef pointf_t<12> pointf12_t;


typedef std::vector<point2_t> vec_of_point2_t;
typedef std::vector<point3_t> vec_of_point3_t;
typedef std::vector<point4_t> vec_of_point4_t;
typedef std::vector<point5_t> vec_of_point5_t;
typedef std::vector<point6_t> vec_of_point6_t;
typedef std::vector<point7_t> vec_of_point7_t;
typedef std::vector<point8_t> vec_of_point8_t;
typedef std::vector<point9_t> vec_of_point9_t;
typedef std::vector<point10_t> vec_of_point10_t;


typedef bgm::box<point2_t> box2_t;
typedef bgm::box<point3_t> box3_t;
typedef bgm::box<point4_t> box4_t;
typedef bgm::box<point5_t> box5_t;
typedef bgm::box<point6_t> box6_t;
typedef bgm::box<point7_t> box7_t;
typedef bgm::box<point8_t> box8_t;
typedef bgm::box<point9_t> box9_t;
typedef bgm::box<point10_t> box10_t;


typedef std::pair<point2_t, size_t> knnq2_t;
typedef std::pair<point3_t, size_t> knnq3_t;
typedef std::pair<point4_t, size_t> knnq4_t;
typedef std::pair<point5_t, size_t> knnq5_t;
typedef std::pair<point6_t, size_t> knnq6_t;
typedef std::pair<point7_t, size_t> knnq7_t;
typedef std::pair<point8_t, size_t> knnq8_t;
typedef std::pair<point8_t, size_t> knnq9_t;
typedef std::pair<point10_t, size_t> knnq10_t;

