#!/bin/bash

DATA_PATH="../data/"
BENCH_BIN_PATH="../build/bin/"

SYN_DATA_PATH="${DATA_PATH}synthetic/"

RESULT_PATH="../results/n/"

mkdir ${RESULT_PATH}

MILLION=1000000

for dist in "uniform" "gaussian" "lognormal"
do
    for n in 1 10 50 100
    do
        for index in "rtree" "rstar" "zm" "mli" "lisa" "fs"
        do
            echo "Benchmark ${index} dataset ${SYN_DATA_PATH}${dist}_${n}m_2_1"
            real_n=$[$n * $MILLION]
            "${BENCH_BIN_PATH}bench2d_${n}m_default" ${index} "${SYN_DATA_PATH}${dist}_${n}m_2_1" $real_n all > "${RESULT_PATH}${index}_${dist}_${n}m"
        done

        for index in "kdtree" "ann" 
        do
            echo "Benchmark ${index} dataset ${SYN_DATA_PATH}${dist}_${n}m_2_1"
            real_n=$[$n * $MILLION]
            "${BENCH_BIN_PATH}bench2d_${n}m_default" ${index} "${SYN_DATA_PATH}${dist}_${n}m_2_1" $real_n knn > "${RESULT_PATH}${index}_${dist}_${n}m"
        done

        for index in "ug" "edg" "ifi" "flood"
        do
            echo "Benchmark ${index} dataset ${SYN_DATA_PATH}${dist}_${n}m_2_1"
            real_n=$[$n * $MILLION]
            "${BENCH_BIN_PATH}bench2d_${n}m_default" ${index} "${SYN_DATA_PATH}${dist}_${n}m_2_1" $real_n range > "${RESULT_PATH}${index}_${dist}_${n}m"
        done
    done
done
