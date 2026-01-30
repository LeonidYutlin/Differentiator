#!/bin/bash

set -xe

LOG_PATH=".log/"

SAVED_DIR=$(pwd)
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd )
cd $SCRIPT_DIR
until test -e ".git" 
do
  cd ..
done

rm -rf $LOG_PATH &&
mkdir $LOG_PATH 
cd $SAVED_DIR
