#!/bin/bash
COMMAND=$@
echo 'Execute command:'
echo 
echo ">>>   $COMMAND   <<<"
echo 
echo 'Press "Y" to comfirm'
read CHOICE
if [ "$CHOICE" == "Y" ]
then
    find . -type d -exec sh -c "(cd {} && $COMMAND)" ';'
fi