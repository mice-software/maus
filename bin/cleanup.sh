#!/bin/sh
# Remove temporary files

if [ "${MAUS_ROOT_DIR}" ]; then
    echo "recursively removing temporary files from ${MAUS_ROOT_DIR}"
    find ${MAUS_ROOT_DIR} -name '*~' -exec rm '{}' \; -print 
    find ${MAUS_ROOT_DIR} -name '*.pyc' -exec rm '{}' \; -print 
    rm -Rf ${MAUS_ROOT_DIR}/doc/html
    rm -Rf ${MAUS_ROOT_DIR}/doc/latex
    rm -f env.sh env.csh
else
    echo "Please define the MAUS_ROOT_DIR environmental variable"
fi
