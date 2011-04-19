#!/bin/bash
./configure
source env.sh
./third_party/build_all.bash 
source env.sh
scons build || exit 1
python -m unittest discover -b -v build || exit 1
