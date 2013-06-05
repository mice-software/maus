#!/usr/bin/env bash

run=04235
filename=${run}.tar
url=http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/04200/${filename}

stagedir=${MAUS_THIRD_PARTY}/third_party/source/
stagefile=${stagedir}/${filename}

destdir=${MAUS_THIRD_PARTY}/third_party/install/share/${run}
destfile=${destdir}/${filename}

# the point of staging to third_party/source/ is that then this will get 
# included in the release tarball (which pulls from third_party/source/)

if [ -n "${MAUS_THIRD_PARTY+x}" ]; then

    if [ -e "${stagefile}" ]
    then
        echo "INFO: Found source archive"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."
        wget ${url} -O "${stagefile}" 
    fi
    if [ -d "$destdir" ]
    then
        echo "INFO: $destdir exists"
    else
        mkdir -p $destdir
        echo "INFO: making $destdir"
    fi
    cp ${stagefile} ${destfile}

    if [ -e "${destfile}" ]
    then
        echo "INFO: Unpacking"
        echo
        cd "${destdir}"
        tar -xf ${filename}
        sleep 1
        echo "INFO: Checking md5sum"
        md5sum -c ${run}.md5 || { echo "FATAL: Failed checksum"; rm "${destdir}"; exit 1; }
        sleep 1
        echo
        echo
        echo "INFO: Data for $run now available for testing against"
    else
        echo "FATAL: Failed to get the data file - giving up">&2
        exit 1;
    fi
else
    echo
    echo "FATAL: MAUS_THIRD_PARTY is not set" >&2
    exit 1
fi
