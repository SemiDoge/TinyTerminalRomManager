@echo off
setlocal

cmake -S . -B build -G Ninja
cd build
ninja

endlocal