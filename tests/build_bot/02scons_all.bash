#!/bin/bash
./configure
source env.sh
scons -c && scons config-full && REQUIREALL=1 scons