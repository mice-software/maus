#!/usr/bin/env bash

# one can use 'set -u' to have bash exit whenever a variable
# is found that isn't defined.  I prefer to catch it, alert
# the user about the problem, then exit with an error code.
# This is more dangerous and requires that people be careful
# when using environmental variables
# set -u

while [[ $# > 1 ]]
do
key="$1"
case $key in
    -j|--num-threads)
    if expr "$2" : '-\?[0-9]\+$' >/dev/null
    then
        MAUS_NUM_THREADS="$2"
    fi
    shift
    ;;
    --use-system-gcc)
    if [ "$2" = true ] || [ "$2" = false ]; then
    USE_SYSTEM_GCC="$2"
    fi
    shift
    ;;
esac
shift
done

# Set default to build GCC as a third party
if [ -z "$USE_SYSTEM_GCC" ]; then
    USE_SYSTEM_GCC=false
fi
if [ "$USE_SYSTEM_GCC" != true ] && [ "$USE_SYSTEM_GCC" != false ]; then
    USE_SYSTEM_GCC=false
fi

if [ -z "$MAUS_NUM_THREADS" ]; then
    MAUS_NUM_THREADS=1
fi

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    # try to get the third_party tarball - if that fails try to download
    # normally
    ${MAUS_ROOT_DIR}/third_party/bash/00get_all_libs.bash
    if [ $? != 0 ]; then
        echo "WARNING: Failed to download the third party tarball. I will try"
        echo "WARNING: to download them individually"
    fi

    # Exit the script if ANY command fails
    set -e

    # Now build libraries
    if [ "$USE_SYSTEM_GCC" = false ]; then
        ${MAUS_ROOT_DIR}/third_party/bash/90gmp.bash -j $MAUS_NUM_THREADS
        ${MAUS_ROOT_DIR}/third_party/bash/91mpfr.bash -j $MAUS_NUM_THREADS
        ${MAUS_ROOT_DIR}/third_party/bash/92mpc.bash -j $MAUS_NUM_THREADS
        ${MAUS_ROOT_DIR}/third_party/bash/93gcc.bash -j $MAUS_NUM_THREADS
    fi
    ${MAUS_ROOT_DIR}/third_party/bash/01python.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/10setuptools.bash
    python ${MAUS_ROOT_DIR}/third_party/check_path.py
    ${MAUS_ROOT_DIR}/third_party/bash/02swig.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/03cmake.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/04pip.bash
    # Install python modules with copies pulled down from web - will get 
    # latest versions and overwrite any from third_party tarball
    ${MAUS_ROOT_DIR}/third_party/bash/38scons.bash
    # ${MAUS_ROOT_DIR}/third_party/bash/39numpy.bash
    ${MAUS_ROOT_DIR}/third_party/bash/40python_extras.bash -gi
    # Make the numpy includes findable
    old_dir=`pwd`
    cd ${MAUS_THIRD_PARTY}/third_party/install/include
    ln -s ../lib/python2.7/site-packages/numpy/core/include/numpy numpy
    cd ${old_dir}

    ${MAUS_ROOT_DIR}/third_party/bash/51xboa.bash
    ${MAUS_ROOT_DIR}/third_party/bash/42libxml2.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/43libxslt.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/44cdb.bash

    # Python and Python site-packages are now built; now JSON and HEP libraries
    ${MAUS_ROOT_DIR}/third_party/bash/52jsoncpp.bash
    ${MAUS_ROOT_DIR}/third_party/bash/20gsl.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/21root.bash -j $MAUS_NUM_THREADS
    echo "Sourcing ROOT..."
    source ${MAUS_ROOT_DIR}/third_party/build/root/bin/thisroot.sh
    echo "Using ROOT:" `which root`
    ${MAUS_ROOT_DIR}/third_party/bash/28xercesc.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/29expat.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/32clhep2.1.1.0.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/35geant4.9.6.bash -j $MAUS_NUM_THREADS
    # resource environment so that g4bl picks up our ROOT env
    source ${MAUS_ROOT_DIR}/env.sh >& /dev/null
    source ${MAUS_ROOT_DIR}/third_party/build/root/bin/thisroot.sh
    echo "ROOTSYS: " $ROOTSYS
    ${MAUS_ROOT_DIR}/third_party/bash/81G4beamline.bash -j $MAUS_NUM_THREADS
    # ${MAUS_ROOT_DIR}/third_party/bash/82heprep.bash

    # Doxygen for code documentation, requires a recent version of flex
    ${MAUS_ROOT_DIR}/third_party/bash/54flex.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/55doxygen.bash -j $MAUS_NUM_THREADS

    # MAUS should now build okay - now for the test and execution environment
    source ${MAUS_ROOT_DIR}/env.sh >& /dev/null
    ${MAUS_ROOT_DIR}/third_party/bash/11gtest.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/53unpacking.bash -j $MAUS_NUM_THREADS
    ${MAUS_ROOT_DIR}/third_party/bash/41cpplint.bash
    ${MAUS_ROOT_DIR}/third_party/bash/45beamline_fieldmaps.bash
    ${MAUS_ROOT_DIR}/third_party/bash/71test_data.bash
    ${MAUS_ROOT_DIR}/third_party/bash/50cloc.bash
    ${MAUS_ROOT_DIR}/third_party/bash/72maus-apps.bash

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

