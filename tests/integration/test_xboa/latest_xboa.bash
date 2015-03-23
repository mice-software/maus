#!/usr/bin/env bash

# Get the latest version of xboa from bzr

set -e

if [[ ${MAUS_THIRD_PARTY} ]]
then
    cd ${MAUS_THIRD_PARTY}/third_party/build/
    rm -r ${MAUS_THIRD_PARTY}/third_party/build/xboa_dev || \
        echo "Failed to remove xboa_dev - it probably does not exist"
    bzr checkout https://code.launchpad.net/~chris-rogers/xboa/sourceforge xboa_dev
    cd xboa_dev
    python setup.py build
    python setup.py install
else
    echo "FATAL: MAUS_THIRD_PARTY not set"
    exit 1
fi

