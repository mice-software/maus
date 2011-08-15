#!/usr/bin/env bash

# add open gl to geant4; not run as default (requires opengl libraries)

directory=geant4.9.2.p04
filename=${directory}.tar.gz
url=http://www.geant4.org/geant4/support/source/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
  cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	echo
  echo "INFO: Building geant4 extras - note this is not a supported operation."
  echo "INFO: Will attempt to add geant4 opengl extensions for visualisation."
  echo
	maus_geant4_setup.py ${MAUS_ROOT_DIR}/third_party/build/${directory}/.config/bin/Linux-g++ ${MAUS_ROOT_DIR}/third_party/build/${directory} ${MAUS_ROOT_DIR}/third_party/install extras
	cp ${MAUS_ROOT_DIR}/third_party/source/Configure .
	
	echo
  echo "INFO: Building:"
	echo
  sleep 1

	./Configure -f .config/bin/Linux-g++/config.sh -d -e -build

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
