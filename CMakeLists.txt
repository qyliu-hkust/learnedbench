cmake_minimum_required(VERSION 3.16)

project(learnedbench)

# check dependencies
find_package(TPIE REQUIRED)


add_subdirectory(utils)


include_directories(${TPIE_INCLUDE_DIRS})

add_executable(bench bench.cpp)

target_link_libraries(bench Utils ${TPIE_LIBRARIES} pthread)
