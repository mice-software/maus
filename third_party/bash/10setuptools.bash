#!/usr/bin/env bash

directory=setuptools-0.6c11
filename=${directory}-py2.7.egg
url=http://pypi.python.org/packages/2.7/s/setuptools/${filename}

echo
echo 'INFO: Installing third party library Python Setup Tools 0.6c11'
echo '--------------------------------------------------------------'
echo

my_prefix=/install
my_destdir=${MAUS_ROOT_DIR}/third_party

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."

        wget --no-check-certificate --directory-prefix="${MAUS_ROOT_DIR}/third_party/source" ${url}
    fi

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd "${MAUS_ROOT_DIR}/third_party/source"
        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Building:"
        echo
        sh ${MAUS_ROOT_DIR}/third_party/source/setuptools-0.6c11-py2.7.egg
    fi

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
