cmake_minimum_required(VERSION 3.16)

project(learnedbench)

# check dependencies
find_package(TPIE REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${TPIE_INCLUDE_DIRS})

add_subdirectory(utils)

add_executable(bench bench.cpp)

target_link_libraries(bench Utils ${TPIE_LIBRARIES} Boost::program_options pthread)
