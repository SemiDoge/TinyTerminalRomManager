@echo off
setlocal

set "EXE=ttrm.exe"

cd build
"%EXE%" %*

endlocal