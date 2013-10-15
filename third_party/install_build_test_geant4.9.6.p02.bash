#!/bin/bash

cmake --version
if [ $? != 0 ]; then
    echo "FATAL: Require cmake to build g4.9.6.p02"
    exit 1
fi

./configure
source env.sh # fails due to no maus-apps installed - it's okay
set -e
./third_party/build_all.bash
source env.sh
bash third_party/bash/29expat.bash
bash third_party/bash/32clhep2.1.1.0.bash
bash third_party/bash/35geant4.9.6.bash
source env_geant4.9.6.p02.sh
scons -c
scons -j8
bash tests/run_tests.bash


