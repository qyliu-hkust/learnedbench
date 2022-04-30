cmake_minimum_required(VERSION 3.16)

project(learnedbench)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


set(CMAKE_CXX_FLAGS "-mbmi2 -O3 -DNDEBUG")
# set(CMAKE_CXX_FLAGS "-mbmi2 -Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(PROFILE "whether starting heap profile" OFF)
option(RSMI "whether building RSMI" OFF)


set(BOOST_ROOT "/home/qiyu/boost_1_79_0")
set(Boost_INCLUDE_DIR "/home/qiyu/boost_1_79_0")
set(Boost_LIBRARY_DIR "/home/qiyu/boost_1_79_0/stage/lib")

SET(EXECUTABLE_OUTPUT_PATH "/home/qiyu/learnedbench/build/bin")

# check dependencies
find_package(TPIE REQUIRED)
find_package(Boost 1.79.0 REQUIRED COMPONENTS program_options system filesystem)
find_package(GEOS REQUIRED)

if(PROFILE)
  find_package(benchmark REQUIRED)
  add_definitions(-DHEAP_PROFILE)
endif()

if(RSMI)
  # the RSMI benchmark needs to be compiled using cxx14
  # the dependency libtorch can be downloaded from 
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  set(TORCH_PATH "/home/qiyu/libtorch")
  list(APPEND CMAKE_PREFIX_PATH TORCH_PATH)
  find_package(Torch REQUIRED PATHS ${TORCH_PATH} CONFIG)

  # RSMI library path
  set(RSMI_PATH "/home/qiyu/learnedbench/indexes/rsmi")

  include_directories(${Boost_INCLUDE_DIR} ${TPIE_INCLUDE_DIRS} ${RSMI_PATH})

  if(PROFILE)
    add_executable(bench_rsmi_hp bench/bench_rsmi.cpp)
    target_link_libraries(bench_rsmi_hp ${TPIE_LIBRARIES} ${TORCH_LIBRARIES} Boost::system Boost::filesystem pthread "${RSMI_PATH}/build/librsmi.a" -ltcmalloc)
  else()
    add_executable(bench_rsmi bench/bench_rsmi.cpp)
    target_link_libraries(bench_rsmi ${TPIE_LIBRARIES} ${TORCH_LIBRARIES} Boost::system Boost::filesystem pthread "${RSMI_PATH}/build/librsmi.a")
  endif()

else()
  # the other benchmarks are compiled using cxx17 (required by PGM-Index)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  # ANN library path
  set(ANN_PATH "/home/qiyu/learnedbench/indexes/ann_1.1.2")

  include_directories(${Boost_INCLUDE_DIR} ${TPIE_INCLUDE_DIRS} ${RSMI_PATH} ${ANN_PATH})

  add_executable(datagen utils/datagen.cpp)
  target_link_libraries(datagen ${TPIE_LIBRARIES} Boost::program_options pthread)

  if(PROFILE)
    add_executable(bench2d_hp bench/bench.cpp)
    target_link_libraries(bench2d_hp ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a" -ltcmalloc)
  else()
    # add_executable(bench2d_default bench/bench.cpp)
    # target_link_libraries(bench2d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_default PUBLIC PARTITION_NUM=100)
    
    # add_executable(bench2d_fs bench/bench.cpp)
    # target_link_libraries(bench2d_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_fs PUBLIC PARTITION_NUM=60)

    # add_executable(bench2d_osm bench/bench.cpp)
    # target_link_libraries(bench2d_osm ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_osm PUBLIC PARTITION_NUM=250)

    # add_executable(bench3d_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20)

    # add_executable(bench4d_default bench/bench.cpp)
    # target_link_libraries(bench4d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench4d_default PUBLIC BENCH_DIM=4 PARTITION_NUM=10)

    # add_executable(bench6d_default bench/bench.cpp)
    # target_link_libraries(bench6d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench6d_default PUBLIC BENCH_DIM=6 PARTITION_NUM=5)

    # add_executable(bench8d_default bench/bench.cpp)
    # target_link_libraries(bench8d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench8d_default PUBLIC BENCH_DIM=8 PARTITION_NUM=4)

    # add_executable(bench10d_default bench/bench.cpp)
    # target_link_libraries(bench10d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench10d_default PUBLIC BENCH_DIM=10 PARTITION_NUM=3)

    # add_executable(bench2d_1m_default bench/bench.cpp)
    # target_link_libraries(bench2d_1m_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_1m_default PUBLIC PARTITION_NUM=23)

    # add_executable(bench2d_10m_default bench/bench.cpp)
    # target_link_libraries(bench2d_10m_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_10m_default PUBLIC PARTITION_NUM=70)

    # add_executable(bench2d_50m_default bench/bench.cpp)
    # target_link_libraries(bench2d_50m_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_50m_default PUBLIC PARTITION_NUM=158)

    # add_executable(bench2d_100m_default bench/bench.cpp)
    # target_link_libraries(bench2d_100m_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_100m_default PUBLIC PARTITION_NUM=223)

    # add_executable(bench2d_e4_default bench/bench.cpp)
    # target_link_libraries(bench2d_e4_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e4_default PUBLIC PARTITION_NUM=100 INDEX_ERROR_THRESHOLD=4)

    # add_executable(bench2d_e16_default bench/bench.cpp)
    # target_link_libraries(bench2d_e16_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e16_default PUBLIC PARTITION_NUM=100 INDEX_ERROR_THRESHOLD=16)

    # add_executable(bench2d_e64_default bench/bench.cpp)
    # target_link_libraries(bench2d_e64_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e64_default PUBLIC PARTITION_NUM=100 INDEX_ERROR_THRESHOLD=64)

    # add_executable(bench2d_e256_default bench/bench.cpp)
    # target_link_libraries(bench2d_e256_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e256_default PUBLIC PARTITION_NUM=100 INDEX_ERROR_THRESHOLD=256)

    # add_executable(bench2d_e1024_default bench/bench.cpp)
    # target_link_libraries(bench2d_e1024_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e1024_default PUBLIC PARTITION_NUM=100 INDEX_ERROR_THRESHOLD=1024)

    # add_executable(bench2d_e4_fs bench/bench.cpp)
    # target_link_libraries(bench2d_e4_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e4_fs PUBLIC PARTITION_NUM=60 INDEX_ERROR_THRESHOLD=4)

    # add_executable(bench2d_e16_fs bench/bench.cpp)
    # target_link_libraries(bench2d_e16_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e16_fs PUBLIC PARTITION_NUM=60 INDEX_ERROR_THRESHOLD=16)

    # add_executable(bench2d_e64_fs bench/bench.cpp)
    # target_link_libraries(bench2d_e64_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e64_fs PUBLIC PARTITION_NUM=60 INDEX_ERROR_THRESHOLD=64)

    # add_executable(bench2d_e256_fs bench/bench.cpp)
    # target_link_libraries(bench2d_e256_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e256_fs PUBLIC PARTITION_NUM=60 INDEX_ERROR_THRESHOLD=256)

    # add_executable(bench2d_e1024_fs bench/bench.cpp)
    # target_link_libraries(bench2d_e1024_fs ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench2d_e1024_fs PUBLIC PARTITION_NUM=60 INDEX_ERROR_THRESHOLD=1024)

    # add_executable(bench3d_e4_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_e4_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_e4_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20 INDEX_ERROR_THRESHOLD=4)

    # add_executable(bench3d_e16_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_e16_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_e16_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20 INDEX_ERROR_THRESHOLD=16)

    # add_executable(bench3d_e64_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_e64_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_e64_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20 INDEX_ERROR_THRESHOLD=64)

    # add_executable(bench3d_e256_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_e256_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_e256_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20 INDEX_ERROR_THRESHOLD=256)

    # add_executable(bench3d_e1024_toronto bench/bench.cpp)
    # target_link_libraries(bench3d_e1024_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    # target_compile_definitions(bench3d_e1024_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20 INDEX_ERROR_THRESHOLD=1024)

    add_executable(bench2d_default bench/bench.cpp)
    target_link_libraries(bench2d_default ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    target_compile_definitions(bench2d_default PUBLIC PARTITION_NUM=100)

    add_executable(bench3d_toronto bench/bench.cpp)
    target_link_libraries(bench3d_toronto ${TPIE_LIBRARIES} Boost::program_options GEOS::geos pthread "${ANN_PATH}/lib/libANN.a")
    target_compile_definitions(bench3d_toronto PUBLIC BENCH_DIM=3 PARTITION_NUM=20)

    
  endif()
endif()

