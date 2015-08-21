#!/bin/bash
counter=$1
TOP=${PWD}
while [ ${counter} -le $2 ]
do
    # STDOUT indicator
    echo "Generate input file No $counter"

    # build files
    num=${counter}
    bname="run-sps-eute-${num}.sh"
    cat > ./$bname << EOF

#!/bin/bash
${TOP}/job-eute.sh run-sps $num $num
EOF
    chmod +x $bname
    # counter increment
    counter=$[${counter}+1]
done