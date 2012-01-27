#!/bin/bash
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

echo "Your current directory is:"
pwd
echo
echo "Your MAUS_ROOT_DIR is:"
echo ${MAUS_ROOT_DIR}
echo
echo "These should agree"


if [ $maus_lcov ]; then
    if [ $maus_lcov -ne "0" ]; then # if set, request coverage stats for cpp using lcov
        if which lcov >& /dev/null; then # check for lcov existence
            echo Clearing lcov
            lcov -q -b ${MAUS_ROOT_DIR} --directory src --zerocounters -q
        else
            echo ERROR: lcov not found - despite environment variable set
            exit 1
        fi
    fi
fi
nosetests --with-coverage -v ${MAUS_ROOT_DIR}/build
if [ $maus_lcov ]; then
    if [ $maus_lcov -ne "0" ]; then
        echo Building lcov output
        lcov  -q -b ${MAUS_ROOT_DIR} --directory src --capture --output-file maus.info
        genhtml -o doc/coverage/ maus.info
    fi
fi

