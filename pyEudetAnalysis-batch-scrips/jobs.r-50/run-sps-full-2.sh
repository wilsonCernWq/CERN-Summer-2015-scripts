#!/bin/bash
cd /afs/cern.ch/work/q/qiwu/PYEUTANA/mathieu/pyEudetAnalysis
source ./setup_CERN.sh
python ./RunFullChain.py -m all -t reconstruction+analysis -o /afs/cern.ch/work/q/qiwu/Data/Simulation_SPS_r50/histo-clic -d /afs/cern.ch/work/q/qiwu/Data/Simulation_SPS_r50/histo-clic -a /afs/cern.ch/work/q/qiwu/Data/Simulation_SPS_r50/histo-clic -i 2 -f 2
