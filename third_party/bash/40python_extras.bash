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

