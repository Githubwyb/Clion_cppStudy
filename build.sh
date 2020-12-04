#!/bin/bash

source envrc

git submodule init && git submodule update

# create folders
rm -rf "$OUTPUTDIR"
mkdir -p "$OUTPUTLIBDIR"

# build lib
# libhv
if [ ! -f "src/threepart/libhv/lib/libhv.a" ]; then
    cd src/threepart/libhv
    bash configure
    make
    cd -
fi
# sdplog
if [ ! -f "src/threepart/spdlog/build/libspdlog.a" ]; then
    mkdir src/threepart/spdlog/build
    cd src/threepart/spdlog/build
    cmake .. && make
    cd -
fi

# build main program
make

if [ -d "./config" ]; then
    cp -a config/* "$OUTPUTDIR"
fi

ln -sf "${OUTPUTDIR}/${BINNAME}" "./${BINNAME}"
