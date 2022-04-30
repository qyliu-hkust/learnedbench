cmake_minimum_required(VERSION 3.16)

project(learnedbench)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_definitions(-DHEAP_PROFILE)

set(CMAKE_CXX_FLAGS "-mbmi2 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "/home/qiyu/boost_1_79_0")
set(Boost_INCLUDE_DIR "/home/qiyu/boost_1_79_0")
set(Boost_LIBRARY_DIR "/home/qiyu/boost_1_79_0/stage/lib")

# check dependencies
find_package(TPIE REQUIRED)
find_package(Boost 1.79.0 REQUIRED COMPONENTS program_options)
find_package(GEOS REQUIRED)
# find_package(benchmark REQUIRED)



# ANN library path
set(ANN_PATH "/home/qiyu/learnedbench/indexes/ann_1.1.2")

include_directories(${Boost_INCLUDE_DIR} ${TPIE_INCLUDE_DIRS} "${ANN_PATH}/include")

add_executable(datagen utils/datagen.cpp)
target_link_libraries(datagen ${TPIE_LIBRARIES} Boost::program_options pthread)

add_executable(bench2d bench/bench2d.cpp)
target_link_libraries(bench2d ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a" -ltcmalloc)

add_executable(test bench/test.cpp)
target_link_libraries(test ${TPIE_LIBRARIES} pthread)
