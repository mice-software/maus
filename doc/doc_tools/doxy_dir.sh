#!/usr/sh
### Very simple script to help speed up doxygenisation for testing purposes
### Run like
###       sh doxy_dir.sh
### and I will put doxygen output in $MICESRC/doc/doxygen. Run like 
###       sh doxy_dir.sh Interface
### and I will just doxygenise the Interface dir (so runs much quicker).
( cat Doxyfile.framework ; echo INPUT=${MAUS_ROOT_DIR}/$1 ) | doxygen -
( cat Doxyfile.backend ; echo INPUT=${MAUS_ROOT_DIR}/$1 ) | doxygen -
echo ""
echo INPUT=${MAUS_ROOT_DIR}/$1
