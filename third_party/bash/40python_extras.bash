#!/usr/bin/env bash

# we use -doesnt_install- easy_install for python packages. Due to instability
# with the easy_install servers mostly these come prepacked. A few have narly
# dependency issues so they need to be called from the easy_install server
# (which has a wider range of dependencies pre-packed)
egg_source=${MAUS_ROOT_DIR}/third_party/source/easy_install
# these are packages in MAUS third party tarball
download_package_list="\
 anyjson python-dateutil>=1.5,<2.0 billiard amqp>=1.0.8,<1.1.0 kombu \
 logilab-common logilab-astng suds validictory nose==1.1 nose-exclude  \
 coverage ipython doxypy pylint==0.25.1 bitarray celery \
 pymongo scons readline numpy==1.5 pylab matplotlib \
"
# these are the packages to install - note the version dependencies
package_list="\
 anyjson python-dateutil billiard amqp kombu \
 logilab-common logilab-astng  suds validictory nose nose-exclude \
 coverage ipython doxypy pylint bitarray celery \
 pymongo scons readline numpy pylab matplotlib \
"
module_test_list="suds validictory nose coverage \
 pylint numpy bitarray pylab matplotlib celery \
 pymongo"
binary_test_list="scons"

# To download the original tarballs and pack them into easy_install dir, try
# running this loop (put a flag on the end of the execution command e.g. run
#     bash 40python_extras.bash
#
if [ "$1" ]; then
    echo "INFO: Attempting to pack eggs for distribution"
    cd $egg_source
    rm *
    for package in $download_package_list
    do
        easy_install -zmaxeb . $package
    done
    downloaded_list=`ls`
    echo "INFO: Downloaded following packages\n$downloaded_list"
    echo "INFO: Packing them now - they can be found tarred in $egg_source"
    for item in $downloaded_list
    do
        tar -czf $item.tar.gz $item
        rm -rf $item
    done
elif [ -n "${MAUS_ROOT_DIR+x}" ]; then
    # first try a local install
    for package in $package_list
    do
        echo "INFO: Installing $package"
        easy_install -H None -f $egg_source $package
    done
    # now check that packages were installed
    for module in $module_test_list
    do
        echo "INFO: Checking import of package $module"
        python -c "import $module" || { echo "FATAL: Failed to install python module $module"; exit 1; }
        echo "INFO: ok"
    done
    for bin in $binary_test_list
    do
        echo "INFO: Checking python script $bin"
        which $bin || { echo "FATAL: Failed to install python script $bin"; exit 1; }
        echo "INFO: ok"
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

