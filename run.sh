#! /bin/bash

cd build

if [ -z "$1" ]
then
    ./romManager
else
    ./romManager $1
fi