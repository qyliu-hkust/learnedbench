#!/bin/bash

DATA_PATH="../data/"
BENCH_BIN_PATH="../build/bin/"

# BENCH4D_DEFAULT="../build/bin/bench4d_default"
# BENCH6D_DEFAULT="../build/bin/bench6d_default"
# BENCH8D_DEFAULT="../build/bin/bench8d_default"
# BENCH10D_DEFAULT="../build/bin/bench10d_default"


SYN_DATA_PATH="${DATA_PATH}synthetic/"

RESULT_PATH="../results/dim/"

mkdir ${RESULT_PATH}

for dist in "uniform" "gaussian" "lognormal"
do
    for k in 2 4 6 8 10
    do
        for index in "rtree" "rstar" "zm" "mli" "lisa" "fs"
        do
            echo "Benchmark ${index} dataset ${dist}_${k}"
            "${BENCH_BIN_PATH}bench${k}d_default" ${index} "${SYN_DATA_PATH}${dist}_20m_${k}_1" 20000000 all > "${RESULT_PATH}${index}_${dist}_${k}"
        done

        for index in "kdtree" "ann" 
        do
            echo "Benchmark ${index} dataset ${dist}_${k}"
            "${BENCH_BIN_PATH}bench${k}d_default" ${index} "${SYN_DATA_PATH}${dist}_20m_${k}_1" 20000000 knn > "${RESULT_PATH}${index}_${dist}_${k}"
        done

        for index in "ug" "edg" "ifi" "flood"
        do
            echo "Benchmark ${index} dataset ${dist}_${k}"
            "${BENCH_BIN_PATH}bench${k}d_default" ${index} "${SYN_DATA_PATH}${dist}_20m_${k}_1" 20000000 range > "${RESULT_PATH}${index}_${dist}_${k}"
        done
    done
done

