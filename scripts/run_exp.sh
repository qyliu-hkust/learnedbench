#!/bin/bash

DATA_PATH="../data/"
BENCH2D_DEFAULT="../build/bin/bench2d_default"
BENCH2D_FS="../build/bin/bench2d_fs"
BENCH2D_OSM="../build/bin/bench2d_osm"

REAL_DATA_PATH="${DATA_PATH}real/"
SYN_DATA_PATH="${DATA_PATH}synthetic/"
DEFAULT_SYN_DATA_PATH="${DATA_PATH}synthetic/Default/"

RESULT_PATH="../results/default/"

mkdir ${RESULT_PATH}

#run experiments on default synthetic datasets
for data in "uniform_20m_2_1" "gaussian_20m_2_1" "lognormal_20m_2_1"
do
    for index in "rtree" "rstar" "zm" "mli" "lisa" "fs"
    do
        echo "Benchmark ${index} dataset ${data}"
        ${BENCH2D_DEFAULT} ${index} "${DEFAULT_SYN_DATA_PATH}$data" 20000000 all > "${RESULT_PATH}${index}_${data}"
    done

    for index in "kdtree" "ann" 
    do
        echo "Benchmark ${index} dataset ${data}"
        ${BENCH2D_DEFAULT} ${index} "${DEFAULT_SYN_DATA_PATH}$data" 20000000 knn > "${RESULT_PATH}${index}_${data}"
    done

    for index in "qdtree" "ug" "edg"  "ifi" "flood"
    do
        echo "Benchmark ${index} dataset ${data}"
        ${BENCH2D_DEFAULT} ${index} "${DEFAULT_SYN_DATA_PATH}$data" 20000000 range > "${RESULT_PATH}${index}_${data}"
    done
done

# run experiments on FourSquare
data="fs"
for index in "rtree" "rstar" "zm" "mli" "lisa"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_FS} ${index} "${REAL_DATA_PATH}$data" 3680126 all > "${RESULT_PATH}${index}_${data}"
done

for index in "kdtree" "ann" 
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_FS} ${index} "${REAL_DATA_PATH}$data" 3680126 knn > "${RESULT_PATH}${index}_${data}"
done

for index in "qdtree" "ug" "edg" "fs" "ifi" "flood"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_FS} ${index} "${REAL_DATA_PATH}$data" 3680126 range > "${RESULT_PATH}${index}_${data}"
done

# run experiments on OSM
data="osm-china"
for index in "rtree" "rstar" "zm" "mli" "lisa"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_OSM} ${index} "${REAL_DATA_PATH}$data" 62734869 all > "${RESULT_PATH}${index}_${data}"
done

for index in "kdtree" "ann" 
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_OSM} ${index} "${REAL_DATA_PATH}$data" 62734869 knn > "${RESULT_PATH}${index}_${data}"
done

for index in "qdtree" "ug" "edg" "fs" "ifi" "flood"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH2D_OSM} ${index} "${REAL_DATA_PATH}$data" 62734869 range > "${RESULT_PATH}${index}_${data}"
done
