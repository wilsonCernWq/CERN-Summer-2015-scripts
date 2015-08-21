#!/bin/bash
top=/afs/cern.ch/work/q/qiwu/Allpix/rundata.r-20/RUN-SPS
dir=/afs/cern.ch/work/q/qiwu/Allpix/allpix/share
idx=1000
if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-cnvt.sh <file-prefix> <Idx>"
    exit
fi
fprefix=$1
num=$2

# STDOUT indicator
echo "converting job No $num"

# build files
source ${dir}/setup_pyLCIO.sh
if [ "${PWD}" != "${top}" ]; then
    cp ${top}/${fprefix}-${num}.tar.gz ./${fprefix}-${num}.tar.gz
fi

python ${dir}/TelescopeConverter.py ./${fprefix}-${num}.tar.gz ./${fprefix}-${num}.slcio ${idx}

if [ "${PWD}" != "${top}" ]; then
    mv ./${fprefix}-${num}.slcio ${top}/${fprefix}-${num}.slcio
fi

exit