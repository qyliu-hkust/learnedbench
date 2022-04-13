project(librsmi)
cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TORCH_PATH "/home/qiyu/libtorch/")
list(APPEND CMAKE_PREFIX_PATH TORCH_PATH)
find_package(Torch REQUIRED PATHS ${TORCH_PATH} CONFIG)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} src_list)

add_library(rsmi ${src_list})

target_link_libraries(rsmi PRIVATE pthread ${TORCH_LIBRARIES})
target_include_directories(rsmi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


