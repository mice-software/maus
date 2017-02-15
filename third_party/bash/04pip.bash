#!/usr/bin/env bash

echo
echo 'INFO: Installing third party library PIP 9.0.1'
echo '----------------------------------------------'
echo

# Setup some variables
wheel_version=0.30.0a0
wheel_directory=wheel-${version}
wheel_filename=${directory}.tar.gz
wheel_url=https://pypi.python.org/packages/a7/37/947b4329c4a3c72093b6c8e9b4be8c7f10c32dbb78848d3a234ce01c059d/${wheel_filename}

pip_version=9.0.1
pip_directory=pip-${pip_version}
pip_filename=${pip_directory}.tar.gz
pip_url=https://pypi.python.org/packages/11/b6/abcb525026a4be042b486df43905d6893fb04f05aac21c32c638e939e447/${pip_filename}

source_dir=${MAUS_THIRD_PARTY}/third_party/source/pip
build_dir=${MAUS_THIRD_PARTY}/third_party/build/pip
install_dir=${MAUS_THIRD_PARTY}/third_party/install

rm -rf ${build_dir}/pip

# Get the source - pip needs wheel as well, so make sure the source is there for that too
# python ${MAUS_ROOT_DIR}/third_party/source/get-pip.py --no-cache-dir -d $source_dir -b $build_dir
if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    # First get the wheel source
    if [ -e "${source_dir}/${wheel_filename}" ]
    then
        echo "INFO: Found source archive in ${source_dir} directory"
    else
        echo "INFO: Pip source archive doesn't exist.  Downloading..."
        wget --no-check-certificate --directory-prefix="${source_dir}" ${wheel_url}
    fi
    if [ -e "${source_dir}/${wheel_filename}" ]
    then
        echo "INFO: Pip source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd "${source_dir}"
        md5sum -c ${wheel_filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${wheel_filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${wheel_filename}; exit 1; }
        sleep 1
    else
        echo "FATAL: Pip source archive still doesn't exist.  Please file a bug report with your operating system,">&2
        echo "FATAL: distribution, and any other useful information at:" >&2
        echo "FATAL: " >&2
        echo "FATAL: http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/" >&2
        echo "FATAL:" >&2
        echo "FATAL: Giving up, sorry..." >&2
        exit 1
    fi

    # Now get the source for pip itself
    if [ -e "${source_dir}/${pip_filename}" ]
    then
        echo "INFO: Found source archive in ${source_dir} directory"
    else
        echo "INFO: Pip source archive doesn't exist.  Downloading..."
        wget --no-check-certificate --directory-prefix="${source_dir}" ${pip_url}
    fi
    if [ -e "${source_dir}/${pip_filename}" ]
    then
        echo "INFO: Pip source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd "${source_dir}"
        md5sum -c ${pip_filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${pip_filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${pip_filename}; exit 1; }
        sleep 1
    else
        echo "FATAL: Pip source archive still doesn't exist.  Please file a bug report with your operating system,">&2
        echo "FATAL: distribution, and any other useful information at:" >&2
        echo "FATAL: " >&2
        echo "FATAL: http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/" >&2
        echo "FATAL:" >&2
        echo "FATAL: Giving up, sorry..." >&2
        exit 1
    fi

    # Install pip, which should install wheel too
    python ${source_dir}/get-pip.py -b $build_dir --prefix $install_dir --no-index -f $source_dir
    pip install wheel -v -b $build_dir --prefix $install_dir -f $source_dir # Just in case
    echo
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
