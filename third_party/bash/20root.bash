#!/bin/bash
# ROOT

directory=root_v5.22.00f
filename=${directory}.source.tar.gz 
url=http://root.cern.ch/download/${filename}


if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "Source archive exists.  Unpacking:"
	rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
	sleep 1
	tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build
	mv ${MAUS_ROOT_DIR}/third_party/build/root ${MAUS_ROOT_DIR}/third_party/build/${directory}
	cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	echo "Configuring:"
	sleep 1
	./configure --enable-roofit --disable-xrootd --disable-krb5 --build=debug --prefix=/install
	echo "Making:"
	sleep 1
	make -j5
	echo "${filename} should be locally build now in your third_party directory, which the rest of MAUS will find."
    else
	echo "Source archive doesn't exist.  Downloading..."

	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

	echo "These next two strings should agree (otherwise the file didn't download properly):"
	md5sum ${MAUS_ROOT_DIR}/third_party/source/${filename}
	cat ${MAUS_ROOT_DIR}/third_party/md5/${filename}.md5 
	
	echo
	echo "but if they don't agree, then please run:"
	echo
	echo "rm ${MAUS_ROOT_DIR}/third_party/source/${filename}"
	echo 
	echo "then download the file manually and put it in the 'source' directory"
	
	if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
	then
	    echo "Source archive exists now, please rerun this command.  Cheers."
	else
	    echo "Source archive still doesn't exist.  Please file a bug report with your operating system, distribution, and any other useful information at:"
	    echo
	    echo "http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/"
	    echo
	    echo "Giving up, sorry..."
	fi
  
fi

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