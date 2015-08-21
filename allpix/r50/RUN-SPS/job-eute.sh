#!/bin/bash
if [[ $# < 2 ]]; then
    echo "Illegal number of parameters"
    echo "Usage:"
    echo "./job-cnvt.sh <file-prefix> <startIdx> <endIdx>"
    exit
fi

while [[ $# > 1 ]]
do
    case $1 in
	-q|--quiet)
	    quiet=1
	    shift
	    ;;
	*)
	    if [[ $# -ne 3 ]]; then
		echo "Illegal number of parameters"
		echo "Usage:"
		echo "./job-cnvt.sh <file-prefix> <startIdx> <endIdx>"
		exit
	    fi
            fprefix=$1
	    counter=$2
	    uplimit=$3
	    shift 3
	    ;;
    esac
done

# build environment 
top=/afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-SPS
dir=/afs/cern.ch/work/q/qiwu/Data/Simulation_SPS_r50
source ${dir}/setup_eutelescope_napliour.sh

while [[ ${counter} -le ${uplimit} ]]
do
    # STDOUT indicator
    echo "converting job No $counter"

    # format run number
    num=$(seq -f "%06g" ${counter} ${counter})

    # copy data
    cp ${top}/${fprefix}-${counter}.slcio ${dir}/lcio-raw/run${num}.slcio
    cp ${dir}/db/run000000-hotpixel-db.slcio ${dir}/db/run${num}-hotpixel-db.slcio

    # do clusearch
    if [[ ${quiet} -ne 1 ]]; then
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg clusearch ${counter}
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg hitmaker  ${counter}
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg fitter    ${counter}
    else
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg clusearch ${counter} &> /dev/null
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg hitmaker  ${counter} &> /dev/null 
	jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg fitter    ${counter} &> /dev/null 
    fi

    # clean up
    rm ${dir}/lcio-raw/run${num}.slcio
    rm ${dir}/results-clic/run${num}-clu.slcio
    rm ${dir}/results-clic/run${num}-hit.slcio
    rm ${dir}/results-clic/run${num}-track.slcio

    # counter increment
    counter=$[${counter}+1]
done
exit