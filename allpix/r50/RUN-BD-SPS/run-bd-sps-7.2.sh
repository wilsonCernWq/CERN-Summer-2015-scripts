#!/bin/bash
# Batch job for run allpix simulation generated by /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-SPS/run-bd-sps.sh at Tue Aug 18 19:41:28 CEST 2015
# ---> do slcio convertion
echo "*(2)***************************************************************************"
echo "*******************************************************************************"
echo "** S-L-C-I-O    F I L E    C O N V E R T I O N ********************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/share/setup_pyLCIO.sh

# convert files
python /afs/cern.ch/work/q/qiwu/Allpix/allpix/share/TelescopeConverter.py ./run-bd-sps-7.tar.gz ./run-bd-sps-7.slcio 1000

# copy data
mv ./run-bd-sps-7.slcio /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-SPS/run-bd-sps-7.slcio
# ---> do eutelescope reconstruction
echo "*(3)***************************************************************************"
echo "*******************************************************************************"
echo "** E U T E L E S C O P E    R E C O N S T R U C T I O N ***********************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/setup_eutelescope_napliour.sh

# copy data
cp /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-BD-SPS/run-bd-sps-7.slcio /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/lcio-raw/run000007.slcio
cp /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/db/run000000-hotpixel-db.slcio /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/db/run000007-hotpixel-db.slcio

# do clusearch
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/reco/clic_timepix_nilou/confQiWu2015.cfg clusearch 7
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/reco/clic_timepix_nilou/confQiWu2015.cfg hitmaker  7
jobsub.py -c /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/reco/clic_timepix_nilou/confQiWu2015.cfg fitter    7

# clean up
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/lcio-raw/run000007.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/results-clic/run000007-clu.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/results-clic/run000007-hit.slcio
rm /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/results-clic/run000007-track.slcio

# ---> do pyEudetAnalysis
echo "*(4)***************************************************************************"
echo "*******************************************************************************"
echo "** P Y E U D E T    A N A L Y S I S *******************************************"
echo "*******************************************************************************"
echo "*******************************************************************************"
source /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis/setup_CERN.sh

# do analysis and reconstruction with pyEudetAnalysis
cd /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis
python ./RunFullChain.py -m DigitalCentroid -t reconstruction+analysis -o /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/histo-clic -d /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/histo-clic -a /afs/cern.ch/work/q/qiwu/Data/Simulation_BD_SPS_r50/histo-clic -i 7 -f 7
