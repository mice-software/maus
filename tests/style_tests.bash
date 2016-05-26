#!/bin/bash
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

echo
echo "INFO: Running the style tests"
echo

python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v ${MAUS_ROOT_DIR}/tests/style

echo
echo "INFO: Style tests complete"
echo
