#!/usr/bin/env bash

# one can use 'set -u' to have bash exit whenever a variable
# is found that isn't defined.  I prefer to catch it, alert
# the user about the problem, then exit with an error code.
# This is more dangerous and requires that people be careful
# when using environmental variables
# set -u

# Exit the script if ANY command fails
set -e

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    filename="third_party_source_v3.tar.gz"
    wget http://micewww.pp.rl.ac.uk:8080/attachments/download/202/${filename} -O ${MAUS_ROOT_DIR}/third_party/source/${filename}

    cd ${MAUS_ROOT_DIR}/third_party/source
    filename=${filename} 
    md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this\
 command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }

    cd ${MAUS_ROOT_DIR}
    tar xvfz third_party/source/${filename}
    

    # One could do the xargs line below, but the problem is error codes.  Xargs doesn't exit nicely
    # (at least the xargs on most machines) if one of the many commands die.  Not using find
    # also happens to make the code a little more readable
    # find ${MAUS_ROOT_DIR}/third_party/bash -type f |grep -v REDUNDANT | sort| xargs -i bash {}


    ${MAUS_ROOT_DIR}/third_party/build_essential.bash
    ${MAUS_ROOT_DIR}/third_party/bash/12setuptools.bash
    ${MAUS_ROOT_DIR}/third_party/bash/20gsl.bash
    ${MAUS_ROOT_DIR}/third_party/bash/21root.bash
    ${MAUS_ROOT_DIR}/third_party/bash/22clhep.bash
    ${MAUS_ROOT_DIR}/third_party/bash/23geant4.bash
    ${MAUS_ROOT_DIR}/third_party/bash/24cloc.bash
    ${MAUS_ROOT_DIR}/third_party/bash/25numpy.bash
    ${MAUS_ROOT_DIR}/third_party/bash/26xboa.bash
    ${MAUS_ROOT_DIR}/third_party/bash/27recpack.bash
    ${MAUS_ROOT_DIR}/third_party/bash/28gtest.bash
    ${MAUS_ROOT_DIR}/third_party/bash/29validictory.bash

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

