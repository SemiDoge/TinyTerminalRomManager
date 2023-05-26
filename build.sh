#! /bin/bash

set -xe
cmake CMakeLists.txt -B build -G Ninja
cd build
ninja