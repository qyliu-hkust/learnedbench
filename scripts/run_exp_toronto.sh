#!/bin/bash

DATA_PATH="../data/"
BENCH3D="../build/bin/bench3d_toronto"


REAL_DATA_PATH="${DATA_PATH}real/"

RESULT_PATH="../results/default/"

mkdir ${RESULT_PATH}

data="toronto"
for index in "rtree" "rstar" "zm" "mli" "lisa"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH3D} ${index} "${REAL_DATA_PATH}$data" 21567172 all > "${RESULT_PATH}${index}_${data}"
done

for index in "kdtree" "ann" 
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH3D} ${index} "${REAL_DATA_PATH}$data" 21567172 knn > "${RESULT_PATH}${index}_${data}"
done

for index in "ug" "edg" "fs" "ifi" "flood"
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH3D} ${index} "${REAL_DATA_PATH}$data" 21567172 range > "${RESULT_PATH}${index}_${data}"
done
