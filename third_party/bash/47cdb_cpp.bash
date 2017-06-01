#!/usr/bin/env bash

version=1.0.4
directory="cdb-C++.v${version}"
filename="cdb.client.api-cpp.v${version}.tgz"
md5file=${filename}.md5

# v1.0.1 source url
#url=http://bazaar.launchpad.net/~janusz-martyniak/mcdb/mice.cdb.client.api-C++/download/head:/cdb.client.apic.v1.0-20170317175958-b4kkvdo0a734708l-1/cdb.client.api-C%2B%2B.v1.0.1.tgz

# v1.0.2 source url
#url=http://bazaar.launchpad.net/~janusz-martyniak/mcdb/mice.cdb.client.api-C++/download/head:/cdb.client.apic.v1.0-20170324143648-18c50lgzb1mcmktm-1/cdb.client.api-C%2B%2B.v1.0.2.tgz

# v1.0.3 source url
## there seems to be some issue with doing a wget of this from the test server
# url=http://bazaar.launchpad.net/~janusz-martyniak/mcdb/mice.cdb.client.api-C++/download/head:/cdb.client.apic.v1.0-20170324164701-p8iga6fy9tvk0417-1/cdb.client.api-C%2B%2B.v1.0.3.tgz

# safer in any case to get the source from micemine
# v1.0.3
url=https://micewww.pp.rl.ac.uk/attachments/download/8954/${filename}

echo
echo 'INFO: Installing third party library CDB-C++' $version
echo '--------------------------------------------------'
echo

while [[ $# > 1 ]]
do
key="$1"
case $key in
    -j|--num-threads)
    if expr "$2" : '-\?[0-9]\+$' >/dev/null
    then
        MAUS_NUM_THREADS="$2"
    fi
    shift
    ;;
esac
shift
done
if [ -z "$MAUS_NUM_THREADS" ]; then
  MAUS_NUM_THREADS=1
fi

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else  
        echo "INFO: Source archive doesn't exist.  Downloading..."

        wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

    fi
  
    echo "Checking for file..... $filename"
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd ${MAUS_ROOT_DIR}/third_party/source
        md5sum -c ${md5file} || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking:"
        echo
        mkdir -p ${MAUS_ROOT_DIR}/third_party/build/${directory}
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build/${directory} > /dev/null
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo
        echo "INFO: Configuring:"
        echo
        sleep 1
        # make sure we can see libpython2.7.so - libxml2 build doesnt seem to see LD_LIBRARY_PATH
        echo
        echo "INFO: Making:"
        echo
        sleep 1
        # generate soap bindings
        make gen
        # build
        make

        # copy library to install location - third_party/install/lib
        LIBDIR=${MAUS_THIRD_PARTY}/third_party/install/lib
        cp lib/libcdbc++.${version}.a ${LIBDIR}

        # copy includes to third_party/install/include/cdb-c++
        INCLUDE_DIR=${MAUS_THIRD_PARTY}/third_party/install/include/cdb-c++
        mkdir -p $INCLUDE_DIR
        cp -r include/* $INCLUDE_DIR

        cd $LIBDIR
        ln -sf libcdbc++.${version}.a libcdbc++.a

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
