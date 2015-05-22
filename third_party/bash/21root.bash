#!/usr/bin/env bash
directory=root_v5.34.30
filename=${directory}.source.tar.gz 
url=ftp://root.cern.ch/root/${filename}

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
    mv ${MAUS_ROOT_DIR}/third_party/build/root ${MAUS_ROOT_DIR}/third_party/build/${directory}
    cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
		
	echo
        echo "INFO: Configuring:"
	echo
	sleep 1

	export ROOTSYS=${MAUS_ROOT_DIR}/third_party/build/${directory}
    x11=${MAUS_THIRD_PARTY}/third_party/install/lib/
    # hack to find third party libraries - for ubuntu et al where they have
    # weird and wonderful library locations to support multiple architectures.
    # Sticks them in ${x11} directory
    python ${MAUS_THIRD_PARTY}/third_party/install/bin/library_finder.py X11 Xext Xft
    ./configure --prefix=${MAUS_ROOT_DIR}/third_party/install --disable-xrootd \
              --enable-gsl-shared --enable-minuit2 \
              --enable-builtin-freetype \
              --with-gsl-incdir=${MAUS_ROOT_DIR}/third_party/install/include \
              --with-gsl-libdir=${MAUS_ROOT_DIR}/third_party/install/lib \
              --with-x11-libdir=${x11} \
              --with-x11-libdir=${x11} \
              --with-xft-libdir=${x11} \
              --with-xext-libdir=${x11}
    #echo
    #echo "INFO: Applying ROOT patch"
    #echo
    #sed 's/PYTHONLIB\s*:=\s*-[a-z0-9.]*/& -pthread -lutil/' < config/Makefile.config > config/Makefile.tmp
    #mv config/Makefile.tmp config/Makefile.config
    #echo
    echo
    echo "INFO: Making:"
    echo
    sleep 1
    make -j$MAUS_NUM_THREADS LDFLAGS="-Wl,--no-as-needed" || { echo "FAIL: Failed to configure/make"; exit 1; }
    make install
    # Create a thisroot.sh link to the MAUS root directory
    ln -fs ${MAUS_ROOT_DIR}/third_party/build/${directory}/bin/thisroot.sh ${MAUS_ROOT_DIR}/thisroot.sh
	            ################################################## 
	echo
        echo "INFO: The package should be locally build now in your"
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
  
else         ################################################## 
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
