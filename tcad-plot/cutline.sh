#!/bin/bash
MIDX=$1
SIDX=$2
FILE=$3
TDIR=$4

source ${TDIR}/envir.sh
FILENAME="test_n_${MIDX}_${SIDX}"
if [ ${SIDX} -lt 10 ]; then
    SIDX="0${SIDX}"
fi
cat > ${PWD}/${FILENAME}.tcl << EOF
# Load data
load_file ${STDB}/PracProj/diode/n2_msh.grd
load_file ${STDB}/PracProj/diode/n${MIDX}_des.tdr_00${SIDX}_diode_des.tdr
create_plot -dataset n${MIDX}_des.tdr_00${SIDX}_diode_des
select_plots {Plot_n${MIDX}_des.tdr_00${SIDX}_diode_des}
# Setup cutlines
set_field_prop -plot Plot_n${MIDX}_des.tdr_00${SIDX}_diode_des -geom n${MIDX}_des.tdr_00${SIDX}_diode_des ElectricField-Y -show_bands
create_cutline -plot Plot_n${MIDX}_des.tdr_00${SIDX}_diode_des -type x -at 50
select_plots {Plot_1}
# Export data
export_curves Curve_1 -plot Plot_1 -filename ${PWD}/${FILENAME}.csv -format csv -overwrite
EOF