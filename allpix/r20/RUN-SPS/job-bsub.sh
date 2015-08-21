#!/bin/bash
if [ "$#" -ne 4 ]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-bsub.sh <file-prefix> <queue> <startIdx> <endIdx>"
    exit
fi
fprefix=$1
counter=$3
while [ ${counter} -le $4 ]
do
    # STDOUT indicator
    echo "submit job No $counter"

    # build files
    num=${counter}
    bsub -q $2 ${fprefix}-${num}.sh

    # counter increment
    counter=$[${counter}+1]
done
exit