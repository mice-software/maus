#!/usr/bin/env bash

echo
echo 'INFO: Installing third party library Cpplint'
echo '--------------------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ `uname -s` == "Darwin" ]; then
        cp ${MAUS_ROOT_DIR}/third_party/source/cpplint.py ${MAUS_ROOT_DIR}/third_party/install/Python.framework/Versions/2.7/lib/python2.7/site-packages/cpplint.py
    else
        cp ${MAUS_ROOT_DIR}/third_party/source/cpplint.py ${MAUS_ROOT_DIR}/third_party/install/lib/python2.7/site-packages/cpplint.py
    fi

    echo "INFO: The package should be locally build now in your"
    echo "INFO: third_party directory, which the rest of MAUS will"
    echo "INFO: find."

else
    echo
    echo "FATAL: MAUS_ROOT_DIR is not set, which is required to" >&2
    echo "FATAL: know where to install this package.  You have two" >&2
    echo "FATAL: options:" >&2
    echo "FATAL:" >&2
    echo "FATAL: 1. Set the MAUS_ROOT_DIR from the command line by" >&2
    echo "FATAL: (if XXX is the directory where MAUS is installed):" >&2
    echo "FATAL:" >&2
    echo "FATAL:        MAUS_ROOT_DIR=XXX ${0}" >&2
    echo "FATAL:" >&2
    echo "FATAL: 2. Run the './configure' script in the MAUS ROOT" >&2
    echo "FATAL: directory, run 'source env.sh' then rerun this" >&2
    echo "FATAL: command ">&2
    exit 1
fi
