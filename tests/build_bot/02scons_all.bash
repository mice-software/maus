#!/bin/bash
./configure
source env.sh
scons -c && scons config-full && scons all