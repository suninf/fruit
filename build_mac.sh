#!/bin/bash

rm -rf xcodebuild
mkdir xcodebuild
cd xcodebuild

# Gen Xcode project
SDKRoot=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_SYSROOT=$SDKRoot -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 -G Xcode ..

cd ..
