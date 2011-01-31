#!/bin/bash
./configure
source env.sh
scons build && python -m unittest discover -b -v build 