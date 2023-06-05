#! /bin/bash

cd build

exe="ttrm"

if [ -z "$1" ]
then
    ./$exe
else
    ./$exe $1
fi