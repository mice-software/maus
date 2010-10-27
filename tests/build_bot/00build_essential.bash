#!/bin/bash
./configure
source env.sh
./third_party/build_essential.bash
scons all