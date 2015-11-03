#!/usr/bin/env bash
version=1.0.0
directory=libmonitor_${version}
filename=${directory}.tarz
url=http://micewww.pp.rl.ac.uk/maus/third_party/${filename}

echo
echo 'INFO: Installing third party library libmonitor 1.0.0'
echo '-----------------------------------------------------'
echo

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
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd "${MAUS_ROOT_DIR}/third_party/source"
        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1

        echo
        echo "INFO: Unpacking:"
        echo
        rm -Rf "${MAUS_ROOT_DIR}/third_party/build/${directory}"
        mkdir "${MAUS_ROOT_DIR}/third_party/build/${directory}"
        rm -Rf "${MAUS_ROOT_DIR}/third_party/install/include/daq"
        mkdir "${MAUS_ROOT_DIR}/third_party/install/include/daq"
        sleep 1
        tar xvf "${MAUS_ROOT_DIR}/third_party/source/${filename}" -C "${MAUS_ROOT_DIR}/third_party/build/${directory}" > /dev/null
        cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/libMDMonitor.a" "${MAUS_ROOT_DIR}/third_party/install/lib"

        echo "INFO: Copying event header from unpacking.."
        cp "${MAUS_ROOT_DIR}/third_party/install/include/unpacking/event.h" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
        cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/monitor.h" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
        cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/MDmonitoring.hh" "${MAUS_ROOT_DIR}/third_party/install/include/daq"

        echo
        echo "INFO: The package should be locally installed now in your"
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
