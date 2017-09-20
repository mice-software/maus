#!/usr/bin/env bash

version=0.3
dirname=maus-apps-${version}
filename=${dirname}.tar.gz
# version 0.2
# url=http://micewww.pp.rl.ac.uk/maus/maus-apps/${filename}
###### version 0.3
# NOTE: older version 0.2 is incompatible with the Django version that comes with MAUS
url=http://micewww.pp.rl.ac.uk/attachments/download/9124/${filename}
sourcedir=${MAUS_THIRD_PARTY}/third_party/source/
sourcefile=${sourcedir}/${filename}
libdir=${MAUS_THIRD_PARTY}/third_party/install/lib/
destdir=${libdir}/${dirname}

echo
echo 'INFO: Installing third party library MAUS Apps' $version
echo '--------------------------------------------------------'
echo

# the point of staging to third_party/source/ is that then this will get 
# included in the release tarball (which pulls from third_party/source/)

echo "INFO: Building ${filename}"
if [ ! -n "${MAUS_THIRD_PARTY+x}" ]; then
    echo "FATAL: MAUS_THIRD_PARTY is not set" >&2
    exit 1
fi

if [ -e "${sourcefile}" ]
then
    echo "INFO: Found source archive ${sourcefile}"
else
    echo "INFO: Source archive doesn't exist.  Downloading..."
    wget ${url} -O "${sourcefile}"  || { echo "FATAL: Download failed"; rm ${sourcefile}; exit 1; }
fi



if [ ! -e "${sourcefile}" ]
then
    echo "FATAL: Failed to get the data file - giving up">&2
    exit 1;
fi


cd ${sourcedir}
echo "INFO: Checking md5sum"
md5sum -c ${filename}.md5 || { echo "FATAL: Failed checksum"; exit 1; }

if [ -e "${destdir}" ]
then
    echo "INFO: $destdir exists; cleaning"
    rm -r $destdir
fi

mkdir -p $destdir
cd "${libdir}"
tar -xzf ${sourcefile} || { echo "FATAL: Failed untar"; exit 1; }
sleep 1
ln -s ${dirname} maus-apps
cd ${dirname}
echo "INFO: Configuring"
./configure --with-maus || { echo "FATAL: Failed configure"; exit 1; }
echo "INFO: $dirname installed successfully at $destdir"

