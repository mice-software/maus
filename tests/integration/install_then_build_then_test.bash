#!/bin/bash
./configure
source env.sh
./third_party/build_all.bash 
source env.sh
scons build || exit 1
./run_tests.bash || exit 1

