#!/bin/bash
# force nosetests here otherwise use easy_install default python - which is
# hard coded to whatever easy_install was set up with (not right if we move code)
# Issue #819
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

# Run scifi tests separately as they have exhibited issues

echo
echo "INFO: Running integration tests : test_scifi"
echo

python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v --nologcapture \
${MAUS_ROOT_DIR}/tests/integration/test_scifi/

echo
echo "INFO: Running the integration tests: main tests"
echo

MAUS_TEST_PLOT_DIR=${MAUS_ROOT_DIR}/tests/integration/plots/ \
python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v \
--exclude-dir tests/integration/test_simulation/test_physics_model_full \
--exclude-dir tests/integration/test_utilities/test_execute_against_data \
--exclude-dir tests/integration/test_utilities/test_geometry \
--exclude-dir tests/integration/test_scifi \
${MAUS_ROOT_DIR}/tests/integration

# We run a few tests separately so captured logging can be turned off
# to stop the splurge of suds output when the preprod cdb is unreachable

echo
echo "INFO: Running integration tests : test_execute_against_data"
echo

python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v --nologcapture \
${MAUS_ROOT_DIR}/tests/integration/test_utilities/test_execute_against_data

echo
echo "INFO: Running integration tests : test_geometry"
echo

python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v --nologcapture \
${MAUS_ROOT_DIR}/tests/integration/test_utilities/test_geometry

echo
echo "INFO: Integration tests complete"
echo
