#!/usr/env bash
# force nosetests here otherwise use easy_install default python - which is 
# hard coded to whatever easy_install was set up with (not right if we move code)
# Issue #819

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi


python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v \
${MAUS_ROOT_DIR}/tests/integration/load_tests

