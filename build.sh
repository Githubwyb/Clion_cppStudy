#!/bin/bash

source envrc

rm -rf build/*
mkdir build/lib

make

cp -a config/* build/
