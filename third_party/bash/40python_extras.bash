#!/usr/bin/env bash

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    #  Just use one version....
    rm -Rf scons-2.1.0.alpha.20110323.tar.gz
    rm -Rf scons-2.1.0.alpha.20110323
    wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source  http://downloads.sourceforge.net/project/scons/scons/2.1.0.alpha.20110323/scons-2.1.0.alpha.20110323.tar.gz
    tar xvfz ${MAUS_ROOT_DIR}/third_party/source/scons-2.1.0.alpha.20110323.tar.gz --directory ${MAUS_ROOT_DIR}/third_party/build/
    easy_install ${MAUS_ROOT_DIR}/third_party/build/scons-2.1.0.alpha.20110323

    easy_install pylint suds validictory nose coverage readline ipython doxypy # numpy pychecker matplotlib
    easy_install "numpy==1.5"
    easy_install "validictory==0.7.0"
    easy_install bitarray
    easy_install matplotlib
    easy_install celery
    easy_install pymongo

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
