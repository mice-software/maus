#!/usr/bin/env bash

# we use -doesnt_install- easy_install for python packages. Due to instability
# with the easy_install servers mostly these come prepacked. A few have narly
# dependency issues so they need to be called from the easy_install server
# (which has a wider range of dependencies pre-packed)
# 
# options:
#
# -c remove any files or directories from $egg_source before doing anything
# -g get packages (will happen after cleaning egg source if -c is set)
# -i install packages (will happen after getting packages if -g is set)
#
# if no options are set, this does nothing(!)

egg_source=${MAUS_THIRD_PARTY}/third_party/source/easy_install
# these are packages in MAUS third party tarball
download_package_list="\
 anyjson python-dateutil>=1.5,<2.0 kombu==2.1.8 \
 logilab-common logilab-astng suds validictory nose==1.1 nose-exclude  \
 coverage ipython doxypy pylint==0.25.1 bitarray celery==2.5.5 \
 pymongo==2.3 readline numpy==1.5 matplotlib==1.1.0 \
"
# these are the packages to install - note the version dependencies
package_list="\
 anyjson python-dateutil kombu \
 logilab-common logilab-astng  suds validictory nose nose-exclude \
 coverage ipython doxypy pylint bitarray celery \
 pymongo scons readline numpy matplotlib \
"
module_test_list="suds validictory nose coverage \
 pylint numpy bitarray matplotlib celery \
 pymongo"
binary_test_list="scons"

cleanup="0"
get_packages="0"
install_packages="0"

while getopts ":cgi" opt; do
  case $opt in
    c)
      cleanup="1"
      ;;
    g)
      get_packages="1"
      ;;
    i)
      install_packages="1"
      ;;
    \?)
      echo "FATAL: Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

if [ -z ${MAUS_THIRD_PARTY} ]; then
    echo "FATAL: MAUS_THIRD_PARTY was not set - aborting"
    exit 1
fi

if [ "$cleanup" == "1" ]; then
    echo "INFO: Cleaning $egg_source directory" 
    cd $egg_source
    rm -r *
fi

if [ "$get_packages" == "1" ]; then
    echo "INFO: Attempting to download eggs and pack for distribution"
    cd $egg_source
    for package in $download_package_list
    do
        easy_install -zmaxeb . $package
    done
    downloaded_list=`ls`
    echo "INFO: Downloaded the following packages"
    echo "$downloaded_list"
    echo "INFO: Packing them now - they can be found tarred in $egg_source"
    for item in $downloaded_list
    do
        tar -czf $item.tar.gz $item
        rm -rf $item
    done
fi

if [ "$install_packages" == "1" ]; then
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

    echo "INFO: easy_install libraries were installed in your third party"
    echo "INFO: directory $MAUS_THIRD_PARTY"
fi

