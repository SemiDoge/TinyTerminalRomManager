set -xe
sed -i 's/#define DEBUG/#define RELEASE/' src/main.cpp
./build.sh
sed -i 's/#define RELEASE/#define DEBUG/' src/main.cpp

sudo cp build/romManager /usr/bin/
mkdir -p ~/.config/romManager
touch ~/.config/romManager/roms.yaml