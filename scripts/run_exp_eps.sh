#!/bin/bash

DATA_PATH="../data/"
BENCH_BIN_PATH="../build/bin/"

SYN_DATA_PATH="${DATA_PATH}synthetic/"
REAL_DATA_PATH="${DATA_PATH}real/"

RESULT_PATH="../results/eps/"

mkdir ${RESULT_PATH}


data="${SYN_DATA_PATH}uniform_20m_2_1"
for eps in 4 16 64 256 1024
do
    for index in "zm" "mli" "lisa" "flood"
    do
        echo "Benchmark ${index} dataset ${data} eps=${eps}"
        "${BENCH_BIN_PATH}bench2d_e${eps}_default" $index $data 20000000 range > "${RESULT_PATH}${index}_uniform_e${eps}"
    done
done

data="${SYN_DATA_PATH}gaussian_20m_2_1"
for eps in 4 16 64 256 1024
do
    for index in "zm" "mli" "lisa" "flood"
    do
        echo "Benchmark ${index} dataset ${data} eps=${eps}"
        "${BENCH_BIN_PATH}bench2d_e${eps}_default" $index $data 20000000 range > "${RESULT_PATH}${index}_gaussian_e${eps}"
    done
done

data="${REAL_DATA_PATH}fs"
for eps in 4 16 64 256 1024
do
    for index in "zm" "mli" "lisa" "flood"
    do
        echo "Benchmark ${index} dataset ${data} eps=${eps}"
        "${BENCH_BIN_PATH}bench2d_e${eps}_fs" $index $data 3680126 range > "${RESULT_PATH}${index}_fs_e${eps}"
    done
done

data="${REAL_DATA_PATH}toronto"
for eps in 4 16 64 256 1024
do
    for index in "zm" "mli" "lisa" "flood"
    do
        echo "Benchmark ${index} dataset ${data} eps=${eps}"
        "${BENCH_BIN_PATH}bench3d_e${eps}_toronto" $index $data 21567172 range > "${RESULT_PATH}${index}_toronto_e${eps}"
    done
done
