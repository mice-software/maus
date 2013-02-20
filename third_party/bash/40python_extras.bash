#!/usr/bin/env bash

egg_source=${MAUS_ROOT_DIR}/third_party/source/easy_install
package_list="suds validictory nose==1.1 nose-exclude coverage readline \
 ipython doxypy pylint==0.25.1 numpy bitarray matplotlib celery \
 pymongo scons"
module_test_list="suds validictory nose coverage readline \
 pylint numpy bitarray matplotlib celery \
 pymongo"
binary_test_list="scons"


if [ "$1" ]; then
    echo "INFO: Attempting to pack eggs for distribution"
    easy_install -H None -zmaxd $egg_source $package_list
elif [ -n "${MAUS_ROOT_DIR+x}" ]; then
#    #  Just use one version....
#    rm -Rf scons-2.1.0.alpha.20110323.tar.gz
#    rm -Rf scons-2.1.0.alpha.20110323
#    wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source  http://downloads.sourceforge.net/project/scons/scons/2.1.0.alpha.20110323/scons-2.1.0.alpha.20110323.tar.gz
#    cd "${MAUS_ROOT_DIR}/third_party/source"
#    md5sum -c scons-2.1.0.alpha.20110323.tar.gz.md5 || { echo "FATAL: Failed to download scons"; echo "FATAL: MD5 checksum failed."; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f scons-2.1.0.alpha.20110323.tar.gz; exit 1; }
#    tar xvfz ${MAUS_ROOT_DIR}/third_party/source/scons-2.1.0.alpha.20110323.tar.gz --directory ${MAUS_ROOT_DIR}/third_party/build/
#    easy_install ${MAUS_ROOT_DIR}/third_party/build/scons-2.1.0.alpha.20110323

    echo "Installing $package_list"

    # first try a local install
    ${MAUS_THIRD_PARTY}/third_party/install/bin/easy_install -H None -f $egg_source $package_list
    for module in $module_test_list
    do
        # fails because of version number on some packages
        python -c "import $module" || { echo "FATAL: Failed to install python module $module"; exit 1; }
    done
    for bin in $binary_test_list
    do
        which $bin || { echo "FATAL: Failed to install python script $bin"; exit 1; }
    done

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

