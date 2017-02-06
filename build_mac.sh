#!/bin/bash

rm -rf makebuild
mkdir makebuild
cd makebuild

# Gen Xcode project
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

cd ..
