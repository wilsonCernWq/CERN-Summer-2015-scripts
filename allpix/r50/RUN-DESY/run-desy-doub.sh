#!/bin/bash
counter=$1
num=${counter} # true counter

dir1=${PWD}
dir2=/afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/jobs.r-50/
while [ ${counter} -le $2 ]
do
    # STDOUT indicator
    echo "Generate input file No $counter"

    # build files
    bname="run-desy-doub-${num}.sh"

    # file2
    ename1="run-desy-eute-${counter}.sh"
    fname1="run-desy-full-${counter}.sh"
    counter=$[${counter}+1]

    # file1
    ename2="run-desy-eute-${counter}.sh"
    fname2="run-desy-full-${counter}.sh"
    counter=$[${counter}+1]

    cat > ./$bname << EOF
#!/bin/bash
${dir1}/${ename1}
${dir2}/${fname1}
${dir1}/${ename2}
${dir2}/${fname2}
EOF
    chmod +x $bname
    num=$[${num}+1]
done