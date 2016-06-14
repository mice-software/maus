#!/usr/bin/env bash

echo
echo 'INFO: Building Event Viewer Package'
echo '-----------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    # Check heprep is present
    heprep=false
    heprep_dir="${MAUS_THIRD_PARTY}/third_party/install/heprep/"
    if [ -e "${heprep_dir}/HepRepXMLWriter.hh" ] && [ -e "${heprep_dir}/HepRepXMLWriter.cc" ]; then
        heprep=true
    fi
    if [ "$heprep" = false ] ; then
        echo
        echo 'FATAL: HepRepXMLWriter not found'
        echo 'FATAL: HepRepXMLWriter can be installed by running third_party/bash/82heprep.bash'
        echo 'FATAL: from your MAUS ***third party*** installation (where MAUS_THIRD_PARTY points)'
        echo
        exit 1
    fi

    echo
    echo 'INFO: Building EVExporter '
    echo

    make -f EVExporter-Makefile

    echo
    echo 'INFO Build Qt Event Viewer'
    echo

    qmake
    make

    echo
    echo 'INFO: Event Viewer Package installation complete'
    echo

else
    echo
    echo 'FATAL: MAUS_ROOT_DIR is not set, which is required to'
    echo 'FATAL: know where to install this package.'
    echo 'FATAL: Enter source env.sh from the the MAUS root directory'
    echo 'FATAL: and try again'
    echo

    exit 1

fi
