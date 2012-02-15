#!/bin/bash
# force nosetests here otherwise use easy_install default python - which is 
# hard coded to whatever easy_install was set up with (not right if we move code)
# Issue #819

arch=`uname -s`

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

if [ "$arch" == "Darwin" ]; then
  NOSETESTS=${MAUS_THIRD_PARTY}/install/Python.framework/Versions/2.7/bin/nosetests
else
  NOSETESTS=${MAUS_THIRD_PARTY}/install/bin/nosetests
fi

MAUS_TEST_PLOT_DIR=${MAUS_ROOT_DIR}/tests/integration/plots/ python ${NOSETESTS} -v ${MAUS_ROOT_DIR}/tests/integration

