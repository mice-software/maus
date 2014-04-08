#!/usr/bin/env bash

# one can use 'set -u' to have bash exit whenever a variable
# is found that isn't defined.  I prefer to catch it, alert
# the user about the problem, then exit with an error code.
# This is more dangerous and requires that people be careful
# when using environmental variables
# set -u

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
    ${MAUS_ROOT_DIR}/third_party/bash/01python.bash
    python ${MAUS_ROOT_DIR}/third_party/check_path.py
    ${MAUS_ROOT_DIR}/third_party/bash/02swig.bash
    ${MAUS_ROOT_DIR}/third_party/bash/10setuptools.bash
    # Install python modules with copies pulled down from web - will get 
    # latest versions and overwrite any from third_party tarball
    ${MAUS_ROOT_DIR}/third_party/bash/39numpy.bash
    ${MAUS_ROOT_DIR}/third_party/bash/40python_extras.bash -gi
    ${MAUS_ROOT_DIR}/third_party/bash/51xboa.bash
    ${MAUS_ROOT_DIR}/third_party/bash/42libxml2.bash
    ${MAUS_ROOT_DIR}/third_party/bash/43libxslt.bash
    ${MAUS_ROOT_DIR}/third_party/bash/44cdb.bash
 
    # python and python site-packages are now built; still missing ROOT from the
    # python environment, that will come later. Now HEP libraries
    ${MAUS_ROOT_DIR}/third_party/bash/20gsl.bash
    ${MAUS_ROOT_DIR}/third_party/bash/21root.bash
    # removed geant 4.9.2
    #${MAUS_ROOT_DIR}/third_party/bash/30clhep.bash
    # added for geant 4.9.6
    ${MAUS_ROOT_DIR}/third_party/bash/29expat.bash
    ${MAUS_ROOT_DIR}/third_party/bash/32clhep2.1.1.0.bash
    ${MAUS_ROOT_DIR}/third_party/bash/35geant4.9.6.bash
    # resource environment so that g4bl picks up our ROOT env
    source ${MAUS_ROOT_DIR}/env.sh >& /dev/null
    ${MAUS_ROOT_DIR}/third_party/bash/81G4beamline.bash
    # removed geant 4.9.2
    #${MAUS_ROOT_DIR}/third_party/bash/31geant4.bash
    ${MAUS_ROOT_DIR}/third_party/bash/52jsoncpp.bash
    # DAQ unpacking library
    ${MAUS_ROOT_DIR}/third_party/bash/53unpacking.bash
    # The used version of doxygen requires a recent version of flex
    ${MAUS_ROOT_DIR}/third_party/bash/54flex.bash
    # Doxygen to make sure the same version is used everywhere
    ${MAUS_ROOT_DIR}/third_party/bash/55doxygen.bash

    # MAUS should now build okay - now for the test and execution environment
    ${MAUS_ROOT_DIR}/third_party/bash/11gtest.bash
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

