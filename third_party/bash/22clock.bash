#!/bin/bash
# ROOT

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
echo "MAUS_ROOT_DIR is not set.  Please run:"
echo 
echo "./configure"
echo "from the top MAUS directory. Then:"
echo 
echo "source env.sh"
echo 
echo "Cheers, bye!"
fi