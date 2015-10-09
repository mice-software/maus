#!/usr/bin/env bash

# require python extras for scons

directory=unpacking-mice
filename=${directory}_3.tarz
url=http://micewww.pp.rl.ac.uk/maus/MAUS_release_version_1.1.0/${filename}

echo
echo 'INFO: Installing third party library Unpacking 3'
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

    cd ${MAUS_ROOT_DIR}/third_party/source

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename} ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}
    fi

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5
    fi

    wget ${url}
    wget ${url}.md5

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

# build Step I unpacking
        echo "INFO: Building StepI unpacker..."
        cd build
        cmake  -DCMAKE_INSTALL_PREFIX=${MAUS_ROOT_DIR}/third_party/install \
               ..
        make -j$MAUS_NUM_THREADS
        make install

# rename libMDUnpack to a StepI name 
        echo "INFO: renaming to StepI library..."
        mv ${MAUS_ROOT_DIR}/third_party/install/lib/libMDUnpack.so ${MAUS_ROOT_DIR}/third_party/install/lib/libMDUnpack_StepI.so

# clean up build area
        rm -rf *
        echo "INFO: Done..."

# build Step IV unpacking with -DSTEPIV_DATA flag on
        echo "INFO: Building StepIV unpacker..."
        cmake  -DCMAKE_INSTALL_PREFIX=${MAUS_ROOT_DIR}/third_party/install -DSTEPIV_DATA=1 \
               ..
        make
        make install

# rename libMDUnpack to a StepIV name 
        echo "INFO: renaming to StepIV library..."
        mv ${MAUS_ROOT_DIR}/third_party/install/lib/libMDUnpack.so ${MAUS_ROOT_DIR}/third_party/install/lib/libMDUnpack_StepIV.so
        rm -rf *
        echo "INFO: Done..."
        echo $LIBS
#       gtest creates soft link from libMDUnpack.so
#       so, ensure that the link exists and points to the correct unpacker version
        cd ${MAUS_THIRD_PARTY}/third_party/install/lib
        if [ "$MAUS_UNPACKER_VERSION" == "StepI" ]; then
           ln -sfn libMDUnpack_StepI.so libMDUnpack.so
        elif [ "$MAUS_UNPACKER_VERSION" == "StepIV" ]; then
           ln -sfn libMDUnpack_StepIV.so libMDUnpack.so
        else
           echo "FATAL: !! Unsupported Unpacker Version !! "
        fi

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
