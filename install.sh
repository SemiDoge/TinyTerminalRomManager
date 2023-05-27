#! /bin/bash

set -xe
sed -i 's/#define DEBUG/#define RELEASE/' inc/constants.h
./build.sh
sed -i 's/#define RELEASE/#define DEBUG/' inc/constants.h

sudo cp build/romManager /usr/bin/
mkdir -p ~/.config/romManager
touch ~/.config/romManager/roms.yaml