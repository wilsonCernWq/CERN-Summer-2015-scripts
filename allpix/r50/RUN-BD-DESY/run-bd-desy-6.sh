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
allpix /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-DESY/run-bd-desy-6.in 0

# compress data
cd ./EUTelescopeFiles/run-bd-desy-6
find . -name '*.txt' -print | tar -cvzf run-bd-desy-6.tar.gz --files-from -
cd -

# copy data
mv run-bd-desy-6.tar.gz .
cp run-bd-desy-6.tar.gz /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-DESY/run-bd-desy-6.tar.gz

# ---> do slcio convertion
echo "*(2)***************************************************************************"
echo "*******************************************************************************"
echo "** S-L-C-I-O    F I L E    C O N V E R T I O N ********************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/share/setup_pyLCIO.sh

# convert files
python /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/TelescopeConverter.py ./run-bd-desy-6.tar.gz ./run-bd-desy-6.slcio 1000

# copy data
mv ./run-bd-desy-6.slcio /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-DESY/run-bd-desy-6.slcio

# ---> do eutelescope reconstruction
echo "*(3)***************************************************************************"
echo "*******************************************************************************"
echo "** E U T E L E S C O P E    R E C O N S T R U C T I O N ***********************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/setup_eutelescope_napliour.sh

# copy data
cp /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-DESY/run-bd-desy-6.slcio /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/lcio-raw/run000006.slcio
cp /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/db/run000000-hotpixel-db.slcio /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/db/run000006-hotpixel-db.slcio

# do clusearch
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/reco/clic_timepix_nilou/confQiWu2015.cfg clusearch 6
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/reco/clic_timepix_nilou/confQiWu2015.cfg hitmaker  6
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/reco/clic_timepix_nilou/confQiWu2015.cfg fitter    6

# clean up
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/lcio-raw/run000006.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/results-clic/run000006-clu.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/results-clic/run000006-hit.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/results-clic/run000006-track.slcio

# ---> do pyEudetAnalysis
echo "*(4)***************************************************************************"
echo "*******************************************************************************"
echo "** P Y E U D E T    A N A L Y S I S *******************************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis/setup_CERN.sh

# do analysis and reconstruction with pyEudetAnalysis
cd /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis
python ./RunFullChain.py -m all -t reconstruction+analysis -o /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/histo-clic -d /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/histo-clic -a /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_DESY_r50/histo-clic -i 6 -f 6
