#!/bin/bash

# *** DEFINE STYLE ***
# [file_start] [file_main_index] [file_sub_index] 
# [legend_start] [legend_i] [legend_f] [legend_s]

# -- Save current directory
TCAD_DIR="/afs/cern.ch/work/q/qiwu/TCAD"
DIR=${PWD}
FUN=${PWD##*/}

# Stop until all parameters used up
until [ -z "$1" ]; do
  case "$1" in
# --- Help 
      -h | --help) 
	  echo "Usage plot_mg [OPTION]... VARIABLES"
	  echo
	  echo "Description:"
	  echo "Plot multiple graph from TCAD file"
	  echo 
	  echo "Option list:"
	  echo "  -h, --help        Print help message"
	  echo "  -d, --directory   Change file directory"
	  echo "  -f, --function    Change default function name"
	  echo "  -t, --tcad        Change TCAD directory"
	  echo "  -q, --quiet       Quiet mode"
	  echo "  -or, --only-root  Calculate only ROOT function"
	  echo "  -oc, --only-cut   Calculate only TCAD data cut"
	  echo 
	  echo "Argument list:y"
	  echo "  [file_prefix] [file_main_index] [file_sub_index]"
	  echo "  [legend_prefix] [legend_i] [legend_f] [legend_s]"
	  shift
	  ;;

# --- Saving directory
      -d | --directory) 
	  DIR=$2; shift 2;;

# --- Setup TCAD directory
      -t | --tcad)
	  TCAD_DIR=$2; shift 2;;

# --- Setup function name
      -f | --function)  
	  FUN=$2; shift 2;;

# --- Mute mode
      -q | --quiet) MUTE_MODE=1; shift;;

# --- Root mode
      -or | --only-root) ROOT_MODE=1; shift;;

# --- Cut mode
      -oc | --only_cut) CUT_MODE=1; shift;;

# --- DEBUG
      --DEBUG) DEBUG_MODE=1; shift;;

# --- Define variables
      *)  F_STA=$1
	  F_M=$2
	  F_S=$3
	  L_STA=$4
	  L_I=$5
	  L_F=$6
	  L_S=$7
	  shift 7
	  ;;
  esac
done

# Setup debug function
if [ "${DEBUG_MODE}" != "" ]; then
    echo
    echo "DEBUG outputs:"
    echo "  Current directory: ${DIR}"
    echo "  TCAD directory:    ${TCAD_DIR}"
    echo "  Function name:     ${FUN}"
    echo "  Mute mode:         ${MUTE_MODE}"
    echo "  ROOT mode:         ${ROOT_MODE}"
    echo "  cut mode:          ${CUT_MODE}"
    exit
fi

# -- Setup counters
F_COUNT=${F_S}
L_COUNT=${L_I} 
let END=${L_F}+${L_S}

# -- Main loop
if [ "${ROOT_MODE}" == "" ]; then
    source ${TCAD_DIR}/envir.sh
    while [ ${L_COUNT} -lt ${END} ]; do
	
# -- Build TCL files
	bash ./cutline.sh ${F_M} ${F_COUNT} ${F_STA} ${TCAD_DIR}
	
# -- Export data
	cd "${TCAD_DIR}"
	if [ "${MUTE_MODE}" != "" ]; then
	    svisual -b ${DIR}/${F_STA}_${F_M}_${F_COUNT}.tcl > /dev/null
	else
	    svisual -b ${DIR}/${F_STA}_${F_M}_${F_COUNT}.tcl
	fi
	cd ${DIR}
	
# --- Output
	let F_COUNT=${F_COUNT}+1
	let L_COUNT=${L_COUNT}+${L_S}
    done
fi

# Running ROOT script
if [ "${CUT_MODE}" == "" ]; then
    root -l "${FUN}.cpp(\"${L_STA}\",\"${F_STA}_${F_M}_\",${F_S},${L_I},${L_F},${L_S})"
fi