#!/usr/bin/env bash
version=`grep "version" $MAUS_ROOT_DIR/README | sed s/\ /_/g`
filename=third_party_libraries_incl_python.tar.gz
url=http://micewww.pp.rl.ac.uk/maus/${version}/${filename}

echo
echo 'INFO: Downloading third party tarball'
echo '-------------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."
        wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/" ${url}
        wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/" ${url}.md5
    fi

    echo "INFO: Source archive exists."
    echo "INFO: Unpacking:"
    echo
    cd ${MAUS_ROOT_DIR}/third_party/
    md5sum -c "${filename}.md5" || { echo "FATAL: Failed to download $url - aborting"; exit 1; }
    tar xvfz ${filename}
else
    echo "ERROR: MAUS_ROOT_DIR was not set"
    echo "ERROR: Need to do (from maus download directory)"
    echo "ERROR:           ./configure"
    echo "ERROR:           source env.sh"
    echo "ERROR: Before attempting to build"
    echo
fi
