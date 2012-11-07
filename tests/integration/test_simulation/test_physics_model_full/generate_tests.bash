#!/usr/bin/env bash

# assumes some particular setup for icool, g4beamline, muon1, maus

export ICOOL_ELMSDB=${HOME}/icool/elmsdb/
for version in 3.20 3.23 3.26
do
    export ICOOL=${HOME}/icool/icool_$version/icool
    export ICOOL_VERSION=$version
    python generate_tests.py icool
    python generate_tests.py icool_elms
done

for version in 2.06 2.08 2.10 2.12-64bit
do
    export G4BL=${HOME}/g4bl/G4beamline-$version-Linux-g++/bin/g4bl
    export G4BL_VERSION=$version
    python generate_tests.py g4bl
done

python generate_tests.py maus

export MUON1_DIR=${HOME}/muon1/work/
export MUON1_VERSION=test
python generate_tests.py muon1

