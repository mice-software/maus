#!/usr/bin/env bash

version=2.1.1.0
directory=clhep-${version}
filename=${directory}.tgz
url=http://proj-clhep.web.cern.ch/proj-clhep/DISTRIBUTION/tarFiles/${filename}

echo
echo 'INFO: Installing third party library CLHEP' $version
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
        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking:"
        echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build > /dev/null
        mv ${MAUS_ROOT_DIR}/third_party/build/${version}/CLHEP ${MAUS_ROOT_DIR}/third_party/build/${directory}
        rmdir ${MAUS_ROOT_DIR}/third_party/build/${version}
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo
        echo "INFO: Configuring:"
        echo
        sleep 1
        ./configure --prefix=${MAUS_ROOT_DIR}/third_party/install
        echo
        echo "INFO: Making:"
        echo
        sleep 1
        make -j$MAUS_NUM_THREADS
        make install
                    ################################################## 
        if [ $? == 0 ]
        then
            echo
            echo "INFO: The package should be locally build now in your"
            echo "INFO: third_party directory, which the rest of MAUS will"
            echo "INFO: find."
        else
            echo "FATAL: Failed to build CLHEP"
            exit 1
        fi
    else
        echo "FATAL: Source archive still doesn't exist.  Please file a bug" >&2
        echo "FATAL: report with your operating system,distribution, and any">&2
        echo "FATAL: other useful information at:" >&2
        echo "FATAL: " >&2
        echo "FATAL: http://micewww.pp.rl.ac.uk/projects/maus/issues/" >&2
        echo "FATAL:" >&2
        echo "FATAL: Giving up, sorry..." >&2
        exit 1
    fi
  
else         ################################################## 
    echo
    echo "FATAL: MAUS_ROOT_DIR is not set, which is required to" >&2  
    echo "FATAL: know where to install this package." >&2
    exit 1
fi

# This is the maximum length of text (ignoring whitespace), 50 chars
##################################################
# this is the maximum amount of characters (including whitespace) per line)
######################################################################
