#!/usr/bin/env bash

directory=G4beamline-2.12-source
filename=${directory}.tgz
url=http://www.muonsinternal.com/muons3/g4beamline/${filename}

echo
echo 'INFO: Installing third party library G4BeamLine 2.12'
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
    -t|--third-party-dir)
    MAUS_THIRD_PARTY="$2"
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
        ls ${MAUS_ROOT_DIR}/third_party/build/${directory}/source
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}/source/bin
        var1="LIBS=\"\$LIBS -lSoXt -lCoin -L/usr/lib -L/usr/X11R6/lib -lGLU -lGL -lXm -lXpm -lXmu -lXt -lXext -lX11 -lXi -lSM -lICE\""
        var2="LIBS=\"\$LIBS -L${MAUS_ROOT_DIR}/third_party/build/root/lib -lSoXt -lCoin -L/usr/lib -L/usr/X11R6/lib -lGLU -lGL -lXm -lXpm -lXmu -lXt -lXext -lX11 -lXi -lSM -lICE\""
        var3="LIBS=\"\$LIBS -lexpat -ldl\""
        var4="LIBS=\"\$LIBS -L/\$G4BL_DIR/../../install/lib -lexpat -ldl\""
        var5='cd \.\./\.\.; rm -f lib/libCLHEP-\[A-Z]\*\.a bin/\[A-Z]\*-config; \\'
        var6='cd \.\./\.\.; rm -f lib/libCLHEP-\[A-Z]\*\.a; \\'

        sed -e "s@$var1@$var2@g" g4bl-config >& g4bl-config2
        sed -e "s@$var3@$var4@g" g4bl-config2 >& g4bl-config3
        mv g4bl-config3 g4bl-config
        chmod a+x g4bl-config

        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sed -e "s@$var5@$var6@g" configure >& configure2
        mv configure2 configure
        chmod a+x configure

        echo
        echo "INFO: Building:"
        echo
        sleep 1

        source ${MAUS_ROOT_DIR}/third_party/build/root/bin/thisroot.sh
        echo $LD_LIBRARY_PATH

        ./configure --disable-visual --with-root=${MAUS_ROOT_DIR}/third_party/build/root/

        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}/source/bin
        mv g4bl-config ${MAUS_ROOT_DIR}/third_party/build/${directory}/bin/g4bl-config
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}

        echo
        echo "INFO: Make:"
        echo
        sleep 1
        make -j$MAUS_NUM_THREADS

        cat ${MAUS_ROOT_DIR}/src//map/MapPyBeamlineSimulation/G4bl/maps/x* > ${MAUS_ROOT_DIR}/src//map/MapPyBeamlineSimulation/G4bl/maps/TypeIBend_6InchGap_tapered.map

        echo
        echo "INFO: The package should be locally build now in your"
        echo "INFO: third_party directory, which the rest of MAUS will"
        echo "INFO: find."
#       ./bin/g4bldata --install

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

# This is the maximum length of text (ignoring whitespace), 50 chars
##################################################
# this is the maximum amount of characters (including whitespace) per line)
######################################################################
