#!/usr/bin/env bash

# we use -doesnt_install- easy_install for python packages. Due to instability
# with the easy_install servers mostly these come prepacked. A few have narly
# dependency issues so they need to be called from the easy_install server
# (which has a wider range of dependencies pre-packed)
# 
# options:
#
# -c remove any files or directories from $source_dir before doing anything
# -g get packages (will happen after cleaning egg source if -c is set); do not
#    overwrite any existing packages
# -i install packages (will happen after getting packages if -g is set)
#
# if no options are set, this does nothing(!)

echo
echo 'INFO: Installing third party library Python Extras'
echo '--------------------------------------------------'
echo

source_dir=${MAUS_THIRD_PARTY}/third_party/source/pip
build_dir=${MAUS_THIRD_PARTY}/third_party/build/pip
install_dir=${MAUS_THIRD_PARTY}/third_party/install

# The packages to download for the MAUS third party tarball
download_package_list="\
 numpy scipy anyjson python-dateutil>=1.5,<2.0 amqplib>=1.0 six>=1.4.0 \
 logilab-common logilab-astng suds validictory nose==1.1 nose-exclude \
 coverage doxypy astroid isort pylint==1.6.3 bitarray \
 pymongo==2.3 readline matplotlib==2.0.0 \
 django==1.9.6 magickwand psutil==3.0.1 celery==2.5.5
"

# The packages to install
# Note: pil is no longer on PyPI so we only use our existing tarball
package_list="\
 numpy scipy anyjson python-dateutil amqplib six \
 logilab-common logilab-astng  suds validictory nose==1.1 nose-exclude \
 coverage doxypy pylint==1.6.3 bitarray \
 pymongo readline matplotlib \
 pil django magickwand psutil celery \
"

# The modules to test
module_test_list="numpy scipy suds validictory nose coverage \
 pylint bitarray matplotlib pymongo \
 Image django magickwand celery" #Image is pil bottom level

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
    echo "INFO: Cleaning $source_dir directory"
    echo "INFO: PIL and cdb are excluded"
    cd $source_dir
    cp PIL-1.1.7.tar.gz PIL-1.1.7.tar.gz.bak
    cp cdb-1.1.7.tar.gz cdb-1.1.7.tar.gz.bak
    rm -r *.tar.gz *.tgz *.zip *.whl 
    cp PIL-1.1.7.tar.gz.bak PIL-1.1.7.tar.gz
    cp cdb-1.1.7.tar.gz.bak cdb-1.1.7.tar.gz
fi

if [ "$get_packages" == "1" ]; then
    echo "INFO: Attempting to download wheels for distribution"
    # pip download --no-cache-dir -d $source_dir -r ${source_dir}/requirements.txt
    for package in $download_package_list
    do
        pip download --no-cache-dir -d $source_dir $package
    done
    echo "INFO: Downloaded the following packages"
    pip freeze
fi

if [ "$install_packages" == "1" ]; then

    # first try a local install
    for package in $package_list
    do
        echo "INFO: Installing $package"
        pip install --no-index --no-cache-dir -f $source_dir -b $build_dir --prefix $install_dir $package
    done

    # now check that packages were installed
    failed_modules=""
    for module in $module_test_list
    do
        echo "INFO: Checking import of package $module"

        python -c "import $module" || { echo "WARNING: Failed to install python module $module from local source"; failed_modules="$failed_modules $module"; }
        echo "INFO: ok"
    done

    # try installing any packages that failed previously, by using the PyPI repository
    for package in $failed_modules
    do
        echo "INFO: Installing $package from PyPI"
        pip install -b $build_dir --prefix $install_dir $package
    done

    # check that packages were installed again
    failed_modules=""
    for module in $module_test_list
    do
        echo "INFO: Checking import of package $module"

        python -c "import $module" || { echo "FATAL: Failed to install python module $module"; exit 1; }
        echo "INFO: ok"
    done

    echo "INFO: pip libraries were installed in your third party"
    echo "INFO: directory $MAUS_THIRD_PARTY"
fi

