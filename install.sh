#! /bin/bash
# Installs on 

exe="ttrm"
config_dir="~/.config/TinyTermRomManager"
eval "full_config_dir=$config_dir" 

set -xe
sed -i 's/#define DEBUG/#define RELEASE/' inc/constants.h
sed -i 's/set(DEBUG True)/set(DEBUG False)/' CMakeLists.txt
./build.sh
sed -i 's/#define RELEASE/#define DEBUG/' inc/constants.h
sed -i 's/set(DEBUG False)/set(DEBUG True)/' CMakeLists.txt

sudo cp build/$exe /usr/bin/
mkdir -p $full_config_dir