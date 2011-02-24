#!/bin/bash
#
# This script creates a local .tar.gz copy
# of the redmine wiki of MAUS. -Tunnell
#
# edit: make it location independent; 
rm -Rf ${MAUS_ROOT_DIR}/doc/wiki
mkdir ${MAUS_ROOT_DIR}/doc/wiki
cd ${MAUS_ROOT_DIR}/doc/wiki
wget --no-directories --html-extension --level=1 --recursive --convert-links --no-parent -p http://micewww.pp.rl.ac.uk/projects/maus/wiki/Page_index
tar cvfz wiki.tar.gz ${MAUS_ROOT_DIR}/doc/wiki


