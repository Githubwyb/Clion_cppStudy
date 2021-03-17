#!/bin/bash

source envrc

# create folders
rm -rf "$OUTPUTDIR"
mkdir -p "$OUTPUTLIBDIR"

if [ 1 -eq 0 ]; then

git submodule init && git submodule update

# build lib
# libhv
if [ ! -f "threepart/libhv/lib/libhv.a" ]; then
    cd threepart/libhv
    bash configure
    make
    if [ "$?" -ne 0 ]; then
        echo "Build libhv failed"
        rm -f "lib/libhv.a"
        exit
    fi
    cd -
fi
# sdplog
if [ ! -f "threepart/spdlog/build/libspdlog.a" ]; then
    mkdir threepart/spdlog/build
    cd threepart/spdlog/build
    cmake .. && make
    if [ "$?" -ne 0 ]; then
        echo "Build libspdlog failed"
        rm -f "threepart/spdlog/build/libspdlog.a"
        exit
    fi
    cd -
fi
# subhook
if [ ! -f "threepart/subhook/libsubhook.so" ]; then
    cd threepart/subhook
    cmake . && make
    if [ "$?" -ne 0 ]; then
        echo "Build libsubhook failed"
        rm -f "threepart/subhook/libsubhook.so"
        exit
    fi
    cd -
fi
cp "threepart/subhook/libsubhook.so" "$OUTPUTLIBDIR"
fi

# build main program
make

if [ -d "./config" ]; then
    cp -a config/* "$OUTPUTDIR"
fi

ln -sf "${OUTPUTDIR}/${BINNAME}" "./${BINNAME}"
