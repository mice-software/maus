#!/bin/bash

cmake --version
if [ $? != 0 ]; then
    echo "FATAL: Require cmake to build g4.9.5.p01"
    exit 1
fi

./configure
source env.sh
./third_party/build_all.bash
source env.sh
bash third_party/bash/32clhep2.1.1.0.bash
bash third_party/bash/33geant4.9.5.bash
bash third_party/bash/34geant4.9.5-data.bash
source env_geant4.9.5.p01.sh
scons -c
scons -j8
bash tests/unit_tests.bash


