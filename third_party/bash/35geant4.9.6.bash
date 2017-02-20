#!/usr/bin/env bash

directory_old=geant4.9.2.p04
directory=geant4.9.6.p02
filename=${directory}.tar.gz
url=http://www.geant4.org/geant4/support/source/${filename}

echo
echo 'INFO: Installing third party library GEANT 4.9.6.p02'
echo '----------------------------------------------------'
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

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd ${MAUS_ROOT_DIR}/third_party/source
        ls ${filename}.md5
        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking geant4 source:"
        echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/source/${directory}
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        mkdir ${MAUS_ROOT_DIR}/third_party/source/${directory}
        mkdir ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/source/

        echo
        echo "INFO: Fix the data download url"
        echo
        sed -i 's/http:\/\/geant4.cern.ch\//http:\/\/cern.ch\/geant4\//g' ${MAUS_ROOT_DIR}/third_party/source/${directory}/cmake/Modules/Geant4InstallData.cmake

        echo
        echo "INFO: Generating make files:"
        echo
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        cmake -DCMAKE_INSTALL_PREFIX=${MAUS_ROOT_DIR}/third_party/install/ -DEXPAT_INCLUDE_DIR=${MAUS_ROOT_DIR}/third_party/install/include -DEXPAT_LIBRARY=${MAUS_ROOT_DIR}/third_party/install/lib/libexpat.so -DXERCESC_ROOT_DIR=${MAUS_ROOT_DIR}/third_party/install -DGEANT4_INSTALL_DATA=ON ${MAUS_ROOT_DIR}/third_party/source/${directory}
        echo
        echo "INFO: Running make:"
        echo
        make -j$MAUS_NUM_THREADS

        echo
        echo "INFO: Installing:"
        echo
        sleep 1
        make install

        echo
        echo "INFO: Cleaning up"
        echo
                ################################################## 
        if [ $? == 0 ]
        then
            cp -r ${MAUS_ROOT_DIR}/third_party/source/${directory}/examples ${MAUS_ROOT_DIR}/third_party/build/${directory}/examples
            # copy examples and original source files to the build directory before destroying source
            cd ${MAUS_ROOT_DIR}/third_party/source/${directory} 
            for d in `find . -name src`; do \cp -fr $d /home/durga/maus-merge/third_party/build/geant4.9.6.p02/$d >& /dev/null; done
            rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory_old}
            rm -Rf ${MAUS_ROOT_DIR}/third_party/source/${directory}
            echo
            echo "INFO: The package should be locally build now in your"
            echo "INFO: third_party directory, which the rest of MAUS will"
            echo "INFO: find."
        else
            echo "FATAL: Failed to build geant4"
            exit 1
        fi
        echo "INFO: making environment"
    else
        echo "FATAL: Source archive still doesn't exist.  Please file a bug ">&2
        echo "FATAL: report with your operating system, distribution, and any" >&2
        echo "FATAL: other useful information at:" >&2
        echo "FATAL:" >& 2
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
