#!/bin/bash
if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-brun.sh <file-prefix> <startIdx> <endIdx>"
    exit
fi
fprefix=$1
counter=$2
while [ ${counter} -le $3 ]
do
    # STDOUT indicator
    echo "run job No $counter"

    # build files
    num=${counter}
    ./${fprefix}-${num}.sh &> ./${fprefix}-${num}.out &

    # counter increment
    counter=$[${counter}+1]
done
exit