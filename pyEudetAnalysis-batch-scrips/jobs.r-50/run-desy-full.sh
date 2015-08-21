#!/bin/bash
top=/afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis
dir=/afs/cern.ch/work/q/qiwu/Data/Simulation_DESY_r50/histo-clic
num=$1
while [ ${num} -le $2 ]
do
    bname="run-desy-full-${num}.sh"
    cat > ${bname} << EOF 
#!/bin/bash
cd ${top}
source ./setup_CERN.sh
python ./RunFullChain.py -m all -t reconstruction+analysis -o ${dir} -d ${dir} -a ${dir} -i ${num} -f ${num}
EOF
    chmod +x ${bname}
    # counter increment
    num=$[${num}+1]
done
