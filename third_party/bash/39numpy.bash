#!/usr/bin/env bash

vers="1.5.1"
filename="numpy-${vers}.tar.gz"
directory="numpy-${vers}"
url="http://sourceforge.net/projects/numpy/files/NumPy/${vers}/${filename}/download"

echo
echo 'INFO: Installing third party library NumPy' $vers
echo '-------------------------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else  
        echo "INFO: Source archive doesn't exist.  Downloading..."
        wget -O${MAUS_ROOT_DIR}/third_party/source/${filename} ${url}
    fi
   
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd ${MAUS_ROOT_DIR}/third_party/source
        md5sum -c ${filename}.md5 || { echo "FATAL: checksum failed on ${filename}." >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking:"
        echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build > /dev/null
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo
        echo "INFO: Installing:"
        echo
        sleep 1
        python setup.py install
        cd ${MAUS_ROOT_DIR}/third_party/install/include/
        ln -s ../lib/python2.7/site-packages/numpy/core/include/numpy/
        cd ${MAUS_ROOT_DIR}/third_party/install/lib/
        ln -s ..//lib/python2.7/site-packages/numpy/core/lib/libnpymath.a
        echo "INFO: Listing of numpy includes and libs"
        sleep 1
        cd ${MAUS_ROOT_DIR}
        ls -lah third_party/install/lib/libnpymath.a
        ls -lah third_party/install/include/numpy
        ls -lah third_party/install/include/numpy/*

        ################################################## 
        echo
        echo "INFO: The package should be locally build now in your"
        echo "INFO: third_party directory, which the rest of MAUS will"
        echo "INFO: find."
    else
        echo "FATAL: Source archive still doesn't exist.  Please file a bug report with your operating system,">&2
        echo "FATAL: distribution, and any other useful information at:" >&2
        echo "FATAL: " >&2
        echo "FATAL: http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/" >&2
        echo "FATAL:" >&2
        echo "FATAL: Giving up, sorry..." >&2
        exit 1
    fi
  
else         ################################################## 
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

# This is the maximum length of text (ignoring whitespace), 50 chars
##################################################
# this is the maximum amount of characters (including whitespace) per line)
######################################################################
