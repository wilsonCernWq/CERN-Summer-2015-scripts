# .bashrc

# setup promote
PS1="[anna&\u@\h \W]\$ "

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# User specific aliases and functions
echo "----------------> Anna my love <----------------"

export WORK=/afs/cern.ch/work/q/qiwu

alias workdir="cd $WORK"

alias homedir="cd /home/qiwu"

alias opendir="nautilus"

GCC_PATH=/afs/cern.ch/sw/lcg/external/gcc/4.8/x86_64-slc6/setup.sh
ROOT5=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.30/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh
ROOT6=/afs/cern.ch/sw/lcg/releases/LCG_74root6/ROOT/6.02.05/x86_64-slc6-gcc48-opt/bin/thisroot.sh

ROOT_EXE5() { source $GCC_PATH; source $ROOT5; root "$@"; }
ROOT_EXE6() { source $GCC_PATH; source $ROOT6; root "$@"; }

alias root6=ROOT_EXE6
alias root5=ROOT_EXE5

DEL_TO_TRASH() { mv "$@" ~/.local/share/Trash/files; }

alias del=DEL_TO_TRASH
