#!/bin/bash
./configure
source env.sh
./third_party/build_all.bash 
scons build
python -m unittest discover -b -v build 
