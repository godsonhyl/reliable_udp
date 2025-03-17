#!/bin/bash

workdir=$(cd $(dirname $0); pwd)

buildDir=$workdir/../build_cmake_linux

if [ ! -d $buildDir ];then
    mkdir -p $buildDir
fi

cd $buildDir
rm -rf *

echo setup linux release in $(pwd)

time1=`date +%H%M%S`

cmake .. -DCMAKE_BUILD_TYPE=Release

make install -j16

time2=`date +%H%M%S`

echo begtime: $time1

echo endtime: $time2

cd -

