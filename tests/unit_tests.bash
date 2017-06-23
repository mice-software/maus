#!/bin/bash
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

echo
echo "INFO: Running the unit tests"
echo

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

# some naughty devs are spitting out junk into cwd from their tests...
here=`pwd`
cd ${MAUS_ROOT_DIR}/tmp/

if [ $maus_lcov ]; then
    if [ $maus_lcov -ne "0" ]; then # if set, request coverage stats for cpp using lcov
        if which lcov >& /dev/null; then # check for lcov existence
            echo Clearing lcov
            lcov -q -b ${MAUS_ROOT_DIR} --directory ${MAUS_ROOT_DIR}/src --zerocounters -q
        else
            echo ERROR: lcov not found - despite environment variable set
            exit 1
        fi
    fi
fi

# force nosetests here otherwise use easy_install default python - which is 
# hard coded to whatever easy_install was set up with (not right if we move code)
# Issue #819

# To add coverage in...
# --with-coverage --cover-html --cover-html-dir=${MAUS_ROOT_DIR}/doc/python_coverage --cover-inclusive
if [ $maus_pycov ]; then
    maus_coverage_extra="--with-coverage --cover-html --cover-html-dir=${MAUS_ROOT_DIR}/doc/python_coverage --cover-inclusive"
fi

python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v ${MAUS_ROOT_DIR}/build $maus_coverage_extra \
--exclude-dir build/test_maus_cpp/test_optics/

python ${MAUS_ROOT_DIR}/src/map/MapCppGlobalTrackMatching/_test_MapCppGlobalTrackMatching.py

echo 'Testing optics separately as it corrupts globals singleton'
python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v ${MAUS_ROOT_DIR}/build/test_maus_cpp/test_optics/ $maus_coverage_extra

if [ $maus_lcov ]; then
    if [ $maus_lcov -ne "0" ]; then
        echo Building lcov output
        lcov  -q -b ${MAUS_ROOT_DIR} --directory ${MAUS_ROOT_DIR}/src --capture --output-file maus.info
        genhtml -o ${MAUS_ROOT_DIR}/doc/cpp_coverage/ maus.info
    fi
fi

cd $here

echo
echo "INFO: Unit tests complete"
echo
