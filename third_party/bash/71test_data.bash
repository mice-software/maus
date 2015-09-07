#!/usr/bin/env bash

stagedir=${MAUS_THIRD_PARTY}/third_party/source/
destdir=${MAUS_THIRD_PARTY}/third_party/install/share/test_data/

# First get the old tracker cosmic gdc data file
echo "INFO: Getting test scifi cosmic data"
gdc_file="gdc1901.001"
wget http://micewww.pp.rl.ac.uk/maus/MAUS_release_version_1.0.0/${gdc_file}
wget http://micewww.pp.rl.ac.uk/maus/MAUS_release_version_1.0.0/${gdc_file}.md5
md5sum -c ${gdc_name}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${gdc_file}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }

if [ -d "$destdir" ]
then
    echo "INFO: $destdir exists"
else
    mkdir -p $destdir
    echo "INFO: making $destdir"
fi

mv $gdc_file $destdir
mv ${gdc_file}.md5 $destdir

# Now get the run data
if [ "$#" -gt 0 ]; then
    # if there are command line arguments it should be a list of run numbers
    # eventually this becomes a load test (but online recon is failing right now)
    run_list=$@
    cat_list=$@
else
    # if there are no command line arguments we generate default data set
    run_list="04234 04258 04235"
    cat_list="04234 04235"
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
    # lazy approach to choosing between Step1 and Step4, we could use
    # the approach run >= 5889 is Step4
    url1=http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/${century}/${filename}
    url2=http://www.hep.ph.ic.ac.uk/micedata/MICE/Step4/${century}/${filename}

    stagefile=${stagedir}/${filename}

    destfile=${destdir}/${filename}

    # the point of staging to third_party/source/ is that then this will get 
    # included in the release tarball (which pulls from third_party/source/)

    if [ -e "${stagefile}" ]
    then
        echo "INFO: Found source archive"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."
        wget ${url1} -O "${stagefile}" 
        if [ $? -ne 0 ]
        then
            # download returned e.g. 404 and put this in ${stagefile}, let's
            # clean up
            rm ${stagefile}
            echo "INFO: Failed to download from Step1 area; try Step4 area"
            wget ${url2} -O "${stagefile}"
            if [ $? -ne 0 ]
            then
                echo "FATAL: Failed to download from Step4 area"
                rm ${stagefile}
                exit 1
            fi
        fi
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

