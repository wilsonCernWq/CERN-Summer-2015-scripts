#!/bin/bash
counter=$1
TOP=${PWD}
while [ ${counter} -le $2 ]
do
    # STDOUT indicator
    echo "Generate input file No $counter"

    # build files
    num=${counter}
    bname="run-desy-${num}.sh"
    optar="run-desy-${num}.tar.gz"
    input="run-desy-${num}.in"
    ofold="./EUTelescopeFiles/run-desy-${num}"
    
    # setup proparties
    touch ./$bname
    chmod +x ./$bname
    cat > ./$bname << EOF
#!/bin/bash
# Batch job for run allpix
# ---> save files locally and then move it back
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/setup_allpix_qiwu_geant4.9.10.sh

# run simulation
allpix ${TOP}/${input} q

# compress data
cd ${ofold}
find . -name '*.txt' -print | tar -cvzf ${optar} --files-from -
mv ${optar} ${TOP}
EOF
    
    # counter increment
    counter=$[${counter}+1]
done