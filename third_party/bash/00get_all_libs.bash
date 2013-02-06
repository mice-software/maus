#!/usr/bin/env bash
version=`grep "version" $MAUS_ROOT_DIR/README | sed s/\ /_/g`
filename=third_party_libraries_incl_python.tar.gz
url=http://micewww.pp.rl.ac.uk/maus/${version}/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."
        wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/source" ${url}
    fi

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Source archive exists."
        echo "INFO: Unpacking:"
        echo
        tar xvfz "${MAUS_ROOT_DIR}/third_party/source/${filename}" -C "${MAUS_ROOT_DIR}/third_party/"
    else
        echo "WARN: Failed to get source archive - build scripts will attempt to"
        echo "WARN: get them from the internet instead..."
        exit 1
    fi
else
    echo "ERROR: MAUS_ROOT_DIR was not set"
    echo "ERROR: Need to do (from maus download directory)"
    echo "ERROR:           ./configure"
    echo "ERROR:           source env.sh"
    echo "ERROR: Before attempting to build"
    echo
fi
