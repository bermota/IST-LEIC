#!/bin/bash

#runTests inputdir outputdir maxthreads numbuckets
if [ "$#" -ne 4 ]; then
    echo "Usage: ${0} inputdir outputdir numthreads numbuckets"
    exit
fi

inputdir=$1
outputdir=$2
numThreads=$3
numBuckets=$4

for input in ${inputdir}/*.txt
do
    mkdir $2 2> /dev/null  # Create outputs folder if it does not exist
    fileName=$(basename "${input}")
    echo InputFile=${fileName} NumThreads=1
    ./tecnicofs-nosync ${input} ${outputdir}/"${fileName}"-1.txt 1 1 | tail -n1

    for i in $(seq 2 1 ${numThreads}) 
    do
        fileName=$(basename "${input}")
        echo "InputFile=${fileName} NumThreads=${i}"
        ./tecnicofs-mutex ${input} ${outputdir}/"${fileName}"-${i}.txt ${i} ${numBuckets} | tail -n1

    done

done
