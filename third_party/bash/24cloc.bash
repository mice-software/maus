#!/usr/bin/env bash

filename=cloc-1.52.pl

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    echo "Fetching from web..."
    dir=${MAUS_ROOT_DIR}/third_party/install/bin
    mkdir -p ${dir}
    rm -f ${dir}/${filename}
    wget --directory-prefix=${dir} http://downloads.sourceforge.net/project/cloc/cloc/v1.52/${filename} 
    chmod +x ${dir}/${filename}
    echo "${filename} should be locally build now in your third_party directory, which the rest of MAUS will find."
else
echo "MAUS_ROOT_DIR is not set, which is required to know where to install this package.  You have two options:"
echo
echo "1. Set the MAUS_ROOT_DIR from the command line by (if XXX is the directory where MAUS is installed):"
echo "        MAUS_ROOT_DIR=XXX ${0}"
echo
echo "2. Run the './configure' script in the MAUS ROOT directory, run 'source env.sh', then rerun this command"
echo
fi