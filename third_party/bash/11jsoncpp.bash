#!/bin/bash
# ROOT

directory=jsoncpp-src-0.5.0
filename=${directory}.tar.gz
url=http://downloads.sourceforge.net/project/jsoncpp/jsoncpp/0.5.0/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "Source archive exists.  Unpacking:"
	rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
	sleep 1
	tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build
	cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	echo "Making:"
	sleep 1
	scons platform=linux-gcc
	echo "Hacking around to install since JsonCpp doesn't have a nice install script"
	mkdir -p ${MAUS_ROOT_DIR}/third_party/install/bin
	mkdir -p ${MAUS_ROOT_DIR}/third_party/install/lib/
	find ${MAUS_ROOT_DIR}/third_party/build/${directory}/libs | grep "\.so" | xargs -i cp {} ${MAUS_ROOT_DIR}/third_party/install/lib/ # libjson.so
	find ${MAUS_ROOT_DIR}/third_party/build/${directory}/libs | grep "\.a" | xargs -i cp {} ${MAUS_ROOT_DIR}/third_party/install/lib/ # libjson.a
	find ${MAUS_ROOT_DIR}/third_party/build/${directory}/libs | grep "\.so" | xargs -i cp {} ${MAUS_ROOT_DIR}/third_party/install/lib/libjson.so
        find ${MAUS_ROOT_DIR}/third_party/build/${directory}/libs | grep "\.a" | xargs -i cp {} ${MAUS_ROOT_DIR}/third_party/install/lib/libjson.a  
	find ${MAUS_ROOT_DIR}/third_party/build/${directory}/bin -type f | xargs -i cp {} ${MAUS_ROOT_DIR}/third_party/install/bin/
	cp -r ${MAUS_ROOT_DIR}/third_party/build/${directory}/include/json ${MAUS_ROOT_DIR}/third_party/install/include
	
	echo
	echo "Running unit tests (this should say PASS!)"
	echo
	test_lib_json
	echo
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