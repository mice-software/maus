#!/bin/bash
./configure
source env.sh
scons && python -m unittest discover -b -v build