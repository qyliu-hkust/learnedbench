#!/bin/bash

DATA_PATH="../data/"
BENCH_RSMI="../build/bin/bench_rsmi"


REAL_DATA_PATH="${DATA_PATH}real/"
SYN_DATA_PATH="${DATA_PATH}synthetic/"
DEFAULT_SYN_DATA_PATH="${DATA_PATH}synthetic/Default/"

RESULT_PATH="../results/default/"

mkdir ${RESULT_PATH}

for data in "uniform_20m_2_1" "gaussian_20m_2_1" "lognormal_20m_2_1"
do
    for index in "rsmi" 
    do
        echo "Benchmark ${index} dataset ${data}"
        ${BENCH_RSMI} ${index} "${DEFAULT_SYN_DATA_PATH}$data" 20000000 all > "${RESULT_PATH}${index}_${data}"
    done
done

data="fs"
for index in "rsmi" 
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH_RSMI} ${index} "${REAL_DATA_PATH}$data" 3680126 all > "${RESULT_PATH}${index}_${data}"
done

data="osm-china"
for index in "rsmi" 
do
    echo "Benchmark ${index} dataset ${data}"
    ${BENCH_RSMI} ${index} "${REAL_DATA_PATH}$data" 62734869 all > "${RESULT_PATH}${index}_${data}"
done
