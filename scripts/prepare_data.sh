#!/bin/bash

DATA_PATH="../data/"
BENCH_BIN="../build/datagen"

mkdir DATA_PATH

REAL_DATA_PATH="${DATA_PATH}real/"
SYN_DATA_PATH="${DATA_PATH}synthetic/"

mkdir $REAL_DATA_PATH
mkdir $SYN_DATA_PATH
mkdir "${SYN_DATA_PATH}Default"



# real data 
echo "Download real datasets..."
wget --load-cookies /tmp/cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=11SaFlD7VifETSgSfsQzKNUrE4SW9Cz47' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=11SaFlD7VifETSgSfsQzKNUrE4SW9Cz47" -O real_data.zip && rm -rf /tmp/cookies.txt

if [ -f real_data.zip ]; then
    unzip -d $REAL_DATA_PATH real_data.zip
    rm real_data.zip
else
    echo "Download failed."
    echo "Please manualy download the dataset from https://drive.google.com/file/d/11SaFlD7VifETSgSfsQzKNUrE4SW9Cz47/view"
    echo "Then unzip it to ${REAL_DATA_PATH}."
fi


# synthetic data
MILLION=1000000
DEFAULT_N=50
DEFAULT_D=2
DEFAULT_S=1

# varying dataset size N
echo "Generate data by varying different N..."
for dist in "uniform" "gaussian" "lognormal"
do
    for n in 1 10 50 100
    do
        fname="${dist}_${n}m_${DEFAULT_D}_${DEFAULT_S}"
        real_n=$[$n * $MILLION] 
        $BENCH_BIN -t gen_data -f $fname --dist $dist -n $real_n -d $DEFAULT_D -s $DEFAULT_S
        mv $fname "${SYN_DATA_PATH}"
    done
done
    
# varying dimension d
echo "Generate data by varying different Dim..."
for dist in "uniform" "gaussian" "lognormal"
do
    for d in 2 3 4 5 10
    do
        fname="${dist}_${DEFAULT_N}m_${d}_${DEFAULT_S}"
        real_n=$[$DEFAULT_N * $MILLION]
        $BENCH_BIN -t gen_data -f $fname --dist $dist -n $real_n -d $d -s $DEFAULT_S
        mv $fname "${SYN_DATA_PATH}"
    done
done

# default setting
echo "Generate default data..."
for dist in "uniform" "gaussian" "lognormal"
do
    fname="${dist}_${DEFAULT_N}m_${DEFAULT_D}_${DEFAULT_S}"
    real_n=$[$DEFAULT_N * $MILLION]
    $BENCH_BIN -t gen_data -f $fname --dist $dist -n $real_n -d $DEFAULT_D -s $DEFAULT_S
    mv $fname "${SYN_DATA_PATH}Default"
done
