#!/usr/bin/env bash

# we use -doesnt_install- easy_install for python packages. Due to instability
# with the easy_install servers mostly these come prepacked. A few have narly
# dependency issues so they need to be called from the easy_install server
# (which has a wider range of dependencies pre-packed)
# 
# options:
#
# -c remove any files or directories from $egg_source before doing anything
# -g get packages (will happen after cleaning egg source if -c is set); do not
#    overwrite any existing packages
# -i install packages (will happen after getting packages if -g is set)
#
# if no options are set, this does nothing(!)

echo
echo 'INFO: Installing third party library Python Extras'
echo '--------------------------------------------------'
echo


egg_source=${MAUS_THIRD_PARTY}/third_party/source/easy_install
# these are packages in MAUS third party tarball
download_package_list="\
 anyjson python-dateutil>=1.5,<2.0 kombu==2.1.8 amqplib>=1.0 six>=1.4.0 \
 logilab-common logilab-astng suds validictory nose==1.1 nose-exclude  \
 coverage ipython doxypy pylint==0.25.1 bitarray \
 celery==2.5.5 pymongo==2.3 readline pyparsing>=1.5.6,!=2.0.0,!=2.0.4 \
 matplotlib==1.5.0 \
 scons==2.4.1 pil django==1.5.1 magickwand psutil==3.0.1
"

# these are the packages to install - note the version dependencies
package_list="\
 anyjson python-dateutil amqplib six kombu \
 logilab-common logilab-astng  suds validictory nose nose-exclude \
 coverage ipython doxypy pylint bitarray celery \
 pymongo scons readline pyparsing matplotlib pil django magickwand psutil
"
# note numpy was installed previously, not in this script. We test it's import
# here for convenience
module_test_list="numpy suds validictory nose coverage \
 pylint bitarray matplotlib celery pymongo scons \
 Image django magickwand" #Image is pil bottom level
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
        echo "easy_install -zmaxeb . $package"
        easy_install -zmaxeb . $package &
        # kill the command after <timeout> seconds
        ((timeout = 120))
        while ((timeout > 0)); do
            sleep 1
            kill -0 $! >& /dev/null
            if [ "$?" == "0" ]; then # pid is running (can be killed)
                ((timeout -= 1))
            else # pid is finished (can't be killed)
                ((timeout = 0))
            fi
        done
        kill -0 $! >& /dev/null
        if [ "$?" == "0" ]; then # pid is running (can be killed)
            echo "easy_install of $package has timed out - killing"
            kill -9 $!
        fi
    done
    downloaded_list=`ls --hide=*.tar.gz`
    echo "INFO: Downloaded the following packages"
    echo "$downloaded_list"
    echo "INFO: Packing them now - they can be found tarred in $egg_source"
    echo "INFO: I will skip any packages which are already packed"
    for item in $downloaded_list
    do
        if [ ! -e $item.tar.gz ]; then # ignore existing tarballs
            echo "INFO:       $item"
            tar -czf $item.tar.gz $item
        else
            echo "INFO:       $item skipped"
        fi
        echo ">>>>>removing $item"
        rm -rf $item # cleanup anything not tarred
    done
fi

if [ "$install_packages" == "1" ]; then
    # first try a local install
    for package in $package_list
    do
        echo
        echo "INFO: Installing $package (local package)"
        easy_install -H None -f $egg_source $package
        echo
        if [[ $package =~ .*scons*. ]]; then
            cwd=$(pwd)
            cd ${MAUS_THIRD_PARTY}/third_party/install/lib
            ln -sf ${MAUS_THIRD_PARTY}/third_party/install/lib/python2.7/site-packages/scons-2.4.1-py2.7.egg/scons-2.4.1 scons
            cd ${cwd}
        fi
    done

    # any packages that failed to install from local sources, now try installing with remote downloads
    for module in $module_test_list
    do
        python -c "import $module" || { echo "INFO: Installing $package (remote package)"; easy_install $module; echo; }
    done


    # now check that packages were installed
    for module in $module_test_list
    do
        echo "INFO: Checking import of package $module"
        if [[ $module =~ .*scons*. ]]; then
            python -c "import SCons" || { echo "FATAL: Failed to install python module $module"; exit 1; }
        else
            python -c "import $module" || { echo "FATAL: Failed to install python module $module"; exit 1; }
        fi
        
        echo "INFO: ok"
    done

    echo "INFO: easy_install libraries were installed in your third party"
    echo "INFO: directory $MAUS_THIRD_PARTY"
fi

