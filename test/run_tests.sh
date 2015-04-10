#!/bin/bash

# $Id$
#
# Simple test script adapted from Matt Dillon's 2.16  release.
# It's harder to do this in a Makefile, so let's use a script.

for i in *.asm
do
  NAME=`basename $i .asm`
  echo
  echo "----- Testing $NAME -----"
  ../bin/dasm $i -f1 -o$NAME.bin -DINEEPROM
  # echo "dasm returned $?"
  cmp -s $NAME.bin $NAME.bin.ref
  if [ $? == 0 ]
  then
    echo "----- good"
  else
    echo "---------- error"
  fi
  ../bin/ftohex 1 $NAME.bin $NAME.hex
  # echo "ftohex returned $?"
  cmp -s $NAME.hex $NAME.hex.ref
  if [ $? == 0 ]
  then
    echo "----- good"
  else
    echo "---------- error"
  fi
done

../bin/dasm define_echo.asm -DEXTERNAL_D_DEFAULT -DEXTERNAL_D_VALUE=127 -MEXTERNAL_M_DEFAULT -MEXTERNAL_M_VALUE=127
