#!/usr/bin/env bash

stagedir=${MAUS_THIRD_PARTY}/third_party/source/
destdir=${MAUS_THIRD_PARTY}/third_party/install/share/test_data/

if [ "$#" -gt 0 ]; then
    # if there are command line arguments it should be a list of run numbers
    # eventually this becomes a load test (but online recon is failing right now)
    run_list=$@
    cat_list=$@
else
    # if there are no command line arguments we generate default data set
    run_list="04234 04258 04235"
    cat_list="04235 04234 04258 04234 04258 04234 04258 04234 04258 04234 04258 04235 04235 04235 04235 04235"
fi

echo "INFO: Building test data set"
echo "INFO: Getting runs numbered "${run_list}
echo

if [ ! -n "${MAUS_THIRD_PARTY+x}" ]; then
    echo
    echo "FATAL: MAUS_THIRD_PARTY is not set" >&2
    exit 1
fi

for run in ${run_list}
do
    filename=${run}.tar
    century="${run:0:3}00"
    url=http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/${century}/${filename}

    stagefile=${stagedir}/${filename}

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

echo "INFO: Concatenating run data for runs "
cd ${destdir}
cat_file=${destdir}/test_data.cat
for item in ${cat_list}
do
    echo "INFO:       ${item}"
    file_list="${file_list} "`ls ${destdir}/${item}.0*`
done
echo "INFO: Following files were found "${file_list}
echo "INFO: concatenating"
cat ${file_list} > ${cat_file}
echo "INFO: Successfully built test data set"

