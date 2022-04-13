cmake_minimum_required(VERSION 3.16)

project(learnedbench)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# check dependencies
find_package(TPIE REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

# ANN library path
set(ANN_PATH "/home/qiyu/learnedbench/indexes/ann_1.1.2")

include_directories(${TPIE_INCLUDE_DIRS} "${ANN_PATH}/include")

add_subdirectory(utils)
add_subdirectory(benchmark)

add_executable(bench bench.cpp)

target_link_libraries(bench Utils Benchmarks ${TPIE_LIBRARIES} Boost::program_options pthread "${ANN_PATH}/lib/libANN.a")


