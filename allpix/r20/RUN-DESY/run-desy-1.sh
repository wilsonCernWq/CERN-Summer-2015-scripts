#!/bin/bash
# Batch job for run allpix
# ---> save files locally and then move it back
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/setup_allpix_qiwu_geant4.9.10.sh

# run simulation
allpix /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-20/RUN-DESY/run-desy-1.in q

# compress data
cd ./EUTelescopeFiles/run-desy-1
find . -name '*.txt' -print | tar -cvzf run-desy-1.tar.gz --files-from -
mv run-desy-1.tar.gz /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-20/RUN-DESY
