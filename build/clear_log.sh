#!/bin/bash

VERBOSE=false

while getopts "v" flag; 
do
  case "$flag" in
    v) VERBOSE=true ;;
  esac
done

if $VERBOSE; 
then
  set -xe
else
  set -e 
fi

LOG_PATH=".log/"

SAVED_DIR=$(pwd)
SCRIPT_DIR=$(cd -- "$(dirname -- "$0")" && pwd)
cd $SCRIPT_DIR
until test -e ".git" 
do
  cd ..
done

rm -rf $LOG_PATH &&
mkdir $LOG_PATH 
cd $SAVED_DIR
