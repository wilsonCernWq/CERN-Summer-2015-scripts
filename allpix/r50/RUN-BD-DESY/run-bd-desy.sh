#!/bin/bash
top=${PWD}
dir="/afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50"
idx=1000
me=`basename "$0"`
me="${me%.*}"

echo -e "Working directory:\t${top}"
echo -e "Data storage directory:\t${dir}"
echo -e "Filename base:\t\t${me}"

num=$1
while [ ${num} -le $2 ]; do
    # STDOUT indicator
    echo "Generating file No: ${num}"

    # build files
    rnum=$(seq -f "%06g" ${num} ${num})
    name="${me}-${num}"
    fsh="${name}.sh"
    ftg="${name}.tar.gz"
    fin="${name}.in"
    flc="${name}.slcio"
    fod="./EUTelescopeFiles/${name}"
    
    # setup proparties
    cat > ./$fsh << EOF
#!/bin/bash
# Batch job for run allpix simulation
# ---> save files locally and then move it back
echo "*(1)***************************************************************************"
echo "*******************************************************************************"
echo "** A L L P I X    S I M U L A T I O N *****************************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/setup_allpix_qiwu_geant4.9.10.sh

# run simulation
allpix ${top}/${fin} 0

# compress data
cd ${fod}
find . -name '*.txt' -print | tar -cvzf ${ftg} --files-from -
cd -

# copy data
mv ${ftg} .
cp ${ftg} ${top}/${ftg}

# ---> do slcio convertion
echo "*(2)***************************************************************************"
echo "*******************************************************************************"
echo "** S-L-C-I-O    F I L E    C O N V E R T I O N ********************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/share/setup_pyLCIO.sh

# convert files
python ${dir}/TelescopeConverter.py ./${ftg} ./${flc} ${idx}

# copy data
mv ./${flc} ${top}/${flc}

# ---> do eutelescope reconstruction
echo "*(3)***************************************************************************"
echo "*******************************************************************************"
echo "** E U T E L E S C O P E    R E C O N S T R U C T I O N ***********************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source ${dir}/setup_eutelescope_napliour.sh

# copy data
cp ${top}/${flc} ${dir}/lcio-raw/run${rnum}.slcio
cp ${dir}/db/run000000-hotpixel-db.slcio ${dir}/db/run${rnum}-hotpixel-db.slcio

# do clusearch
jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg clusearch ${num}
jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg hitmaker  ${num}
jobsub.py -c ${dir}/reco/clic_timepix_nilou/confQiWu2015.cfg fitter    ${num}

# clean up
rm ${dir}/lcio-raw/run${rnum}.slcio
rm ${dir}/results-clic/run${rnum}-clu.slcio
rm ${dir}/results-clic/run${rnum}-hit.slcio
rm ${dir}/results-clic/run${rnum}-track.slcio

# ---> do pyEudetAnalysis
echo "*(4)***************************************************************************"
echo "*******************************************************************************"
echo "** P Y E U D E T    A N A L Y S I S *******************************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis/setup_CERN.sh

# do analysis and reconstruction with pyEudetAnalysis
cd /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis
python ./RunFullChain.py -m all -t reconstruction+analysis -o ${dir}/histo-clic -d ${dir}/histo-clic -a ${dir}/histo-clic -i ${num} -f ${num}
EOF
    chmod +x ./$fsh
    
    # counter increment
    num=$[${num}+1]
done