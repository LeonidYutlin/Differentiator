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

build() {
  local DEFINES="-D _DEBUG -D DISABLE_NEWLINES"
  local CFLAGS="-ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=65536 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
  local SRC_FILES="-I src/ src/data_structures/queue/queue.cpp src/data_structures/tree/nodetype.cpp src/diff/io.cpp src/misc/util.cpp src/diff/derivative.cpp src/data_structures/tree/tree.cpp src/data_structures/tree/dump/dump.cpp src/main.cpp src/data_structures/tree/node.cpp src/error/error.cpp src/diff/context.cpp"
  local LIBS="" #empty for now
  local OUTPUT_PATH="build/diff" 
  
  rm -f $OUTPUT_PATH
  g++ $DEFINES $CFLAGS $SRC_FILES $LIBS -o $OUTPUT_PATH
}

#walks up from script dir until is in the same directory as the first arg is
walkUp() {
  local SCRIPT_DIR=$(cd -- "$(dirname -- "$0")" && pwd)
  cd $SCRIPT_DIR
  until test -e "$1" 
  do
    cd ..
  done
}

#actual script
LOG_PATH=".log/.last-build.txt"
SAVED_DIR=$(pwd)
walkUp ".git"
rm -f $LOG_PATH
build |& tee $LOG_PATH
cd $SAVED_DIR

