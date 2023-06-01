#! /bin/bash
# Installs on 

set -xe
sed -i 's/#define DEBUG/#define RELEASE/' inc/constants.h
sed -i 's/set(DEBUG True)/set(DEBUG False)/' CMakeLists.txt
./build.sh
sed -i 's/#define RELEASE/#define DEBUG/' inc/constants.h
sed -i 's/set(DEBUG False)/set(DEBUG True)/' CMakeLists.txt

sudo cp build/romManager /usr/bin/
mkdir -p ~/.config/romManager
touch ~/.config/romManager/roms.yaml