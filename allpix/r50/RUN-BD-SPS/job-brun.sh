#!/bin/bash
if [ "$#" -lt 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-brun.sh <file-prefix> <startIdx> <endIdx> <AdditionalArgs>"
    exit
fi
fprefix=$1
counter=$2
uplimit=$3
shift 3
while [ ${counter} -le $uplimit ]
do
    # STDOUT indicator
    echo "run job No $counter"

    # build files
    num=${counter}
    ./${fprefix}-${num}.sh &> ./${fprefix}-${num}.out "$@"

    # counter increment
    counter=$[${counter}+1]
done
exit