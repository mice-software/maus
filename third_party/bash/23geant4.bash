#!/usr/bin/env bash

http://geant4.cern.ch/support/source/geant4.9.2.p04.tar.gz

directory=geant4.9.2.p04
filename=${directory}.tar.gz 
url=http://geant4.cern.ch/support/source/${filename}


if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "Found source archive in 'source' directory"
    else
	echo "Source archive doesn't exist.  Downloading..."

	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

    fi
   
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "Source archive exists."
        echo "Checking MD5 checksum (otherwise the file didn't download properly):"
        md5sum -c ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 || { echo "FATAL: Failed to download ${filename}. MD5 checksum failed."  >&2; rm -f ${MAUS_ROOT_DIR}/third_party/source/${filename}; exit 1; }

	echo "Unpacking:"
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build
        mv ${MAUS_ROOT_DIR}/third_party/build/root ${MAUS_ROOT_DIR}/third_party/build/${directory}
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo "Configuring:"
        sleep 1
        ./configure --enable-roofit --disable-fftw3 --disable-xrootd --disable-krb5 --build=debug --with-gsl-incdir=${MAUS_ROOT_DIR}/third_party/install/include --with-gsl-libdir=${MAUS_ROOT_DIR}/third_party/install/lib --prefix=${MAUS_ROOT_DIR}/third_party/install
        echo "Making:"
        sleep 1
        make -j5
	make install
        echo "${filename} should be locally build now in your third_party directory, which the rest of MAUS will find."
    else
	echo "Source archive still doesn't exist.  Please file a bug report with your operating system, distribution, and any other useful information at:"
	echo
	echo "http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/"
	echo
	echo "Giving up, sorry..."
    fi
  
else
echo "MAUS_ROOT_DIR is not set, which is required to know where to install this package.  You have two options:"
echo
echo "1. Set the MAUS_ROOT_DIR from the command line by (if XXX is the directory where MAUS is installed):"
echo "        MAUS_ROOT_DIR=XXX ${0}"
echo
echo "2. Run the './configure' script in the MAUS ROOT directory, run 'source env.sh', then rerun this command"
echo
fi