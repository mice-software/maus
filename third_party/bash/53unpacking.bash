#!/usr/bin/env bash

# require python extras for scons

directory=unpacking-mice
filename=${directory}_2.tarz

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    cd ${MAUS_ROOT_DIR}/third_party/source

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename} ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}
    fi

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5
    fi

    wget http://dpnc.unige.ch/~yordan/${filename}
    wget http://dpnc.unige.ch/~yordan/${filename}.md5

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename} ] &&
       [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 ]
    then
        echo "INFO: Source archive file is download."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo

        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking:"
        echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build > /dev/null
#         cp SConstruct_unpacking ${MAUS_ROOT_DIR}/third_party/build/${directory}/SConstruct
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo
        echo "INFO: Making and installing"
        echo
        sleep 1
#         scons install

        cd build
        cmake  -DCMAKE_INSTALL_PREFIX=${MAUS_ROOT_DIR}/third_party/install \
               ..
        make
        make install

        echo
        echo "INFO: The package should be locally build now in your"
        echo "INFO: third_party directory, which the rest of MAUS will"
        echo "INFO: find."
    else
        echo "FATAL: Unable to download the source archive files. Please file a bug report at:" >&2
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
