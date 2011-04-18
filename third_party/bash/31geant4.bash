#!/usr/bin/env bash

directory=geant4.9.2.p04
filename=${directory}.tar.gz
url=http://www.geant4.org/geant4/support/source/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "INFO: Found source archive in 'source' directory"
    else  
	echo "INFO: Source archive doesn't exist.  Downloading..."

	rm -f ${MAUS_ROOT_DIR}/third_party/source/geant_config.tar.gz 
	rm -f ${MAUS_ROOT_DIR}/third_party/source/geant_data.tar.gz 
	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}
	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source http://micewww.pp.rl.ac.uk:8080/attachments/download/92/geant_config.tar.gz # TODO fixme
	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source http://micewww.pp.rl.ac.uk:8080/attachments/download/81/geant_data.tar.gz  # TODO fixme
	#wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source http://micewww.pp.rl.ac.uk:8080/attachments/download/98/Configure # todo fixme

    fi
   
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "INFO: Source archive exists."
	echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
	echo "INFO: download properly):"
	echo
        cd ${MAUS_ROOT_DIR}/third_party/source
	ls ${filename}.md5
	md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
	md5sum -c geant_data.tar.gz.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: geant_data.tar.gz." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }  # TODO fixme
	md5sum -c geant_config.tar.gz.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: geant_config.tar.gz." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; } # todo fixme
	sleep 1
	echo
	echo "INFO: Unpacking:"
	echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build > /dev/null
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	cp ${MAUS_ROOT_DIR}/third_party/source/DeflateOutputStreamBuffer.h source/visualization/HepRep/include/cheprep/DeflateOutputStreamBuffer.h

	echo
	echo "INFO: Applying patch."
	echo 
	grep cstdio source/visualization/HepRep/include/cheprep/DeflateOutputStreamBuffer.h || exit 1
	
	echo
	echo "INFO: Unpacking data"
	echo
	tar xvfz ${MAUS_ROOT_DIR}/third_party/source/geant_data.tar.gz > /dev/null

	echo
        echo "INFO: Unpacking config"
        echo
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/geant_config.tar.gz > /dev/null
	mkdir ${MAUS_ROOT_DIR}/third_party/build/${directory}/.config/bin/Darwin-g++
	sed 's/Linux/Darwin/g' < ${MAUS_ROOT_DIR}/third_party/build/${directory}/.config/bin/Linux-g++/config.sh.old > ${MAUS_ROOT_DIR}/third_party/build/${directory}/.config/bin/Darwin-g++/config.sh.old
	maus_geant4_setup.py ${MAUS_ROOT_DIR}/third_party/build/${directory}/.config/bin/`uname -s`-g++ ${MAUS_ROOT_DIR}/third_party/build/${directory} ${MAUS_ROOT_DIR}/third_party/install
	cp ${MAUS_ROOT_DIR}/third_party/source/Configure .
	
	echo
        echo "INFO: Building:"
	echo
        sleep 1

	./Configure -f .config/bin/`uname -s`-g++/config.sh -d -e -build

	echo
        echo "INFO: Installing:"
	echo
        sleep 1
	./Configure -install
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
