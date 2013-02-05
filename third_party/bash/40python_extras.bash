#!/usr/bin/env bash

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    #  Just use one version....
    rm -Rf scons-2.1.0.alpha.20110323.tar.gz
    rm -Rf scons-2.1.0.alpha.20110323
    wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source  http://downloads.sourceforge.net/project/scons/scons/2.1.0.alpha.20110323/scons-2.1.0.alpha.20110323.tar.gz
    cd "${MAUS_ROOT_DIR}/third_party/source"
    md5sum -c scons-2.1.0.alpha.20110323.tar.gz.md5 || { echo "FATAL: Failed to download scons"; echo "FATAL: MD5 checksum failed."; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f scons-2.1.0.alpha.20110323.tar.gz; exit 1; }
    tar xvfz ${MAUS_ROOT_DIR}/third_party/source/scons-2.1.0.alpha.20110323.tar.gz --directory ${MAUS_ROOT_DIR}/third_party/build/
    easy_install ${MAUS_ROOT_DIR}/third_party/build/scons-2.1.0.alpha.20110323

    package_list="suds validictory nose nose-exclude coverage readline ipython \
                 doxypy pylint==0.25.1 numpy==1.5 bitarray matplotlib celery \
                 pymongo" 
    echo "Installing $package_list"
    easy_install -i ${MAUS_ROOT_DIR}/third_party/source/easy_install/ \
                 -f http://pypi.python.org/simple/ \
                 $package_list
#    easy_install "pylint==0.25.1"
#    easy_install "numpy==1.5"

#    easy_install "validictory==0.7.0"
#    easy_install bitarray
#    easy_install matplotlib
#    easy_install "celery==2.5.5"
#    easy_install pymongo

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
