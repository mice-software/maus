#!/usr/bin/env bash

target=${MAUS_ROOT_DIR}/third_party/install/share/Geant4-9.5.1/data/
source=${MAUS_ROOT_DIR}/third_party/source/
mkdir -pv ${target}
if [ $? != 0 ]; then
    echo "FATAL: Failed to make Geant4 data target directory"
    echo "     ${target}"
    echo "FATAL: Exiting"
    exit 1;
fi

for data_source in G4ABLA.3.0 G4EMLOW.6.23 G4NDL.3.13 PhotonEvaporation.2.2 G4RadioactiveDecay.3.4
do
    cd ${source}
    filename=${data_source}.tar.gz
    md5sum -c ${filename}.md5
    if [ $? != 0 ]; then
        echo "INFO: Failed to find source file - I will try to download it"
        url=http://www.geant4.org/geant4/support/source/${filename}
        wget --directory-prefix=${source} ${url}
    fi
    md5sum -c ${filename}.md5
    if [ $? != 0 ]; then
        echo "FATAL: md5sum failed on ${filename}. Exiting"
        exit 1 # should probably clean up
    fi
    tar  -C ${target} -xzf ${source}/${filename}
    if [ $? != 0 ]; then
        echo "FATAL: Failed to extract ${filename}. Exiting"
        exit 1
    fi
done
