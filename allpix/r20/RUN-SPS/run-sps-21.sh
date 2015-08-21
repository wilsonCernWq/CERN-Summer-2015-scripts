#!/bin/bash
# Batch job for run allpix
# ---> save files locally and then move it back
source /afs/cern.ch/work/q/qiwu/Allpix/allpix/setup_allpix_qiwu_geant4.9.10.sh

# run simulation
allpix /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-20/RUN-SPS/run-sps-21.in 0

# compress data
cd ./EUTelescopeFiles/run-sps-21
find . -name '*.txt' -print | tar -cvzf run-sps-21.tar.gz --files-from -
mv run-sps-21.tar.gz /afs/cern.ch/work/q/qiwu/Allpix/rundata.r-20/RUN-SPS
