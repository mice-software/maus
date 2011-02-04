#!/bin/bash
./configure
source env.sh
./third_party/build_all.bash 
scons build || exit 1
python -m unittest discover -b -v build || exit 1
