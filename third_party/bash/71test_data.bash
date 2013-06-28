#!/usr/bin/env bash

run_list="04234 04258 04235"
if [ ! -n "${MAUS_THIRD_PARTY+x}" ]; then
    echo
    echo "FATAL: MAUS_THIRD_PARTY is not set" >&2
    exit 1
fi


for run in ${run_list}
do
    filename=${run}.tar
    url=http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/04200/${filename}

    stagedir=${MAUS_THIRD_PARTY}/third_party/source/
    stagefile=${stagedir}/${filename}

    destdir=${MAUS_THIRD_PARTY}/third_party/install/share/test_data/
    destfile=${destdir}/${filename}

    # the point of staging to third_party/source/ is that then this will get 
    # included in the release tarball (which pulls from third_party/source/)

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
        cd "${destdir}"
        tar -xf ${filename}
        sleep 1
        echo "INFO: Checking md5sum"
        md5sum -c ${run}.md5 || { echo "FATAL: Failed checksum"; rm "${destdir}"; exit 1; }
        sleep 1
        echo
        echo "INFO: Data for $run now available for testing against"
    else
        echo "FATAL: Failed to get the data file - giving up">&2
        exit 1;
    fi
done

echo "INFO: Concatenating run data"
cat_file=${destdir}/test_data.cat
if [ -e ${cat_file} ]
then
    rm ${cat_file}
fi
touch ${destdir}/test_data.cat
for item in ${run_list}
do
    file_list=`ls ${destdir}/${item}.0*`
    for a_file in ${file_list}
    do
        cat ${cat_file} ${a_file} >& ${cat_file}.tmp
        mv ${cat_file}.tmp ${cat_file}
    done
done
echo "INFO: Following files were downloaded"
ls ${destdir}

