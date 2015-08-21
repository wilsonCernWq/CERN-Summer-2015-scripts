#!/bin/bash
top=/afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-SPS
dir=/afs/cern.ch/work/q/qiwu/Allpix/allpix/share
idx=1000
if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-cnvt.sh <file-prefix> <startIdx> <endIdx>"
    exit
fi
fprefix=$1
counter=$2
while [ ${counter} -le $3 ]
do
    # STDOUT indicator
    echo "converting job No $counter"

    # build files
    num=${counter}
    source ${dir}/setup_pyLCIO.sh
    if [ "${PWD}" != "${top}" ]; then
	cp ${top}/${fprefix}-${num}.tar.gz ./${fprefix}-${num}.tar.gz
    fi
    python ${dir}/TelescopeConverter.py ./${fprefix}-${num}.tar.gz ./${fprefix}-${num}.slcio ${idx}
    if [ "${PWD}" != "${top}" ]; then
	mv ./${fprefix}-${num}.slcio ${top}/${fprefix}-${num}.slcio
    fi
    # counter increment
    counter=$[${counter}+1]
done
exit