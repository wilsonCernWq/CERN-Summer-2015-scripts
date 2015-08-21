#!/bin/bash
# Batch job for run allpix
# ---> save files locally and then move it back
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/setup_allpix_qiwu_geant4.9.10.sh

# run simulation
allpix /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-DESY/run-desy-34.in q

# compress data
cd ./EUTelescopeFiles/run-desy-34
find . -name '*.txt' -print | tar -cvzf run-desy-34.tar.gz --files-from -
mv run-desy-34.tar.gz /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-50/RUN-DESY
