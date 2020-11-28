#!/bin/bash

source envrc

# create folders
rm -rf build/*
mkdir build/parser

# build lib
# libhv
cd src/threepart/libhv
bash configure
make
cd -

# build main program
make

cp -a config/* build/
