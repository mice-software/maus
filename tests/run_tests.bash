#!/bin/bash
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

echo
echo "INFO: Running the full MAUS test suite"
echo

bash ${MAUS_ROOT_DIR}/tests/unit_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

bash ${MAUS_ROOT_DIR}/tests/style_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

bash ${MAUS_ROOT_DIR}/tests/application_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

