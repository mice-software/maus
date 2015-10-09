#!/usr/bin/env bash

version=0.3.2
directory=daq-${version}

#filename=monitor_test.tar
#url=http://micewww.pp.rl.ac.uk/attachments/1187/monitor_test.tar

filename=libmonitor.tarz
# url=http://dpnc.unige.ch/~yordan/libmonitor.tarz
url=http://micewww.pp.rl.ac.uk/maus/MAUS_release_version_1.0.0/libmonitor.tarz

echo
echo 'INFO: Installing third party library DAQ' $version
echo '--------------------------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ] &&
       [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}.md5" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."
        rm -f "${MAUS_ROOT_DIR}/third_party/source/${filename}"
        rm -f "${MAUS_ROOT_DIR}/third_party/source/${filename}.md5"

	wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/source" ${url}
	wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/source" ${url}.md5

    fi

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ] &&
       [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}.md5" ]
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
        mkdir "${MAUS_ROOT_DIR}/third_party/install/include/daq"
	sleep 1
        tar xvf "${MAUS_ROOT_DIR}/third_party/source/${filename}" -C "${MAUS_ROOT_DIR}/third_party/build/${directory}" > /dev/null
        #cd "${MAUS_ROOT_DIR}/third_party/build/${directory}"
	#sleep 1
	#scons install

        cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/libMDMonitor.a" "${MAUS_ROOT_DIR}/third_party/install/lib"
	#cat MDmonitoring.hh |sed "s;#include \"monitor.h\";//#include \"monitor.h\";" | sed "s;#include \"event.h\";//#include \"event.h\";" > tmp.hh
	#mv tmp.hh MDmonitoring.hh
	#cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/event.h" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
        echo "INFO: Copying event header from unpacking.."
	cp "${MAUS_ROOT_DIR}/third_party/install/include/unpacking/event.h" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
	cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/monitor.h" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
	cp "${MAUS_ROOT_DIR}/third_party/build/${directory}/MDmonitoring.hh" "${MAUS_ROOT_DIR}/third_party/install/include/daq"
	#cd -
	#rm -Rf "${MAUS_ROOT_DIR}/third_party/build/${directory}"
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
