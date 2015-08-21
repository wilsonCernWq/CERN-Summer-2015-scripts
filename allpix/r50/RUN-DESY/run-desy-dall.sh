#!/bin/bash
counter=$1
dir1=${PWD}
dir2=/afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/jobs.r-50/
while [ ${counter} -le $2 ]
do
    # STDOUT indicator
    echo "Generate input file No $counter"

    # build files
    num=${counter}
    bname="run-desy-dall-${num}.sh"
    ename="run-desy-eute-${num}.sh"
    fname="run-desy-full-${num}.sh"

    cat > ./$bname << EOF

#!/bin/bash
${dir1}/${ename}
${dir2}/${fname}
EOF
    chmod +x $bname
    # counter increment
    counter=$[${counter}+1]
done