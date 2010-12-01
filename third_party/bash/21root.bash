#!/usr/bin/env bash

directory=root_v5.22.00f
filename=${directory}.source.tar.gz 
url=http://root.cern.ch/download/${filename}


if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "INFO: Found source archive in 'source' directory"
    else  
	echo "INFO: Source archive doesn't exist.  Downloading..."

	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

    fi
   
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "INFO: Source archive exists."
	echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
	echo "INFO: download properly):"
	echo
        cd ${MAUS_ROOT_DIR}/third_party/source
	md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
	sleep 1
	echo
	echo "INFO: Unpacking:"
	echo
        rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
        sleep 1
        tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build > /dev/null
        mv ${MAUS_ROOT_DIR}/third_party/build/root ${MAUS_ROOT_DIR}/third_party/build/${directory}
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	echo
	echo "INFO: If applicable, performing libXpm symbolic link since"
	echo "INFO: old versions of Scientific Linux and ROOT don't work"
	echo "INFO: together.  Aren't you glad we have Scientific Linux?"    
	echo

	rm -f ${MAUS_ROOT_DIR}/third_party/install/lib/libXpm.so # in-case it exists
	extra=""
	if [ -e "/usr/lib64/libXpm.so.4" ] # sl55 64bit
	then
	    echo "INFO: Applying SL5 64-bit libXpm fix since ROOT/SL suck."
	    ln -s /usr/lib64/libXpm.so.4 ${MAUS_ROOT_DIR}/third_party/install/lib/libXpm.so # known ROOT/SL4 bug since 2003 (!!)
	    extra="--with-xpm-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
	elif [ -e "/usr/lib/libXpm.so.4" ] # sl55 32bit
	then
	    echo "INFO: Applying SL5 32-bit libXpm fix since ROOT/SL suck."
	    ln -s /usr/lib/libXpm.so.4 ${MAUS_ROOT_DIR}/third_party/install/lib/libXpm.so # known ROOT/SL4 bug since 2003 (!!)
            extra="--with-xpm-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        elif [ -e "/usr/X11R6/lib64/libXpm.so.4" ] # sl48 64bit
        then
            echo "INFO: Applying SL4 64-bit libXpm fix since ROOT/SL suck."
#	    find /usr/X11R6/lib64/libX*so | xargs -i cp -s {}  ${MAUS_ROOT_DIR}/third_party/install/lib
#            extra="--with-xpm-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        elif [ -e "/usr/X11R6/lib/libXpm.so.4" ] # sl48 32bit
        then
            echo "INFO: Applying SL4 32-bit libXpm fix since ROOT/SL suck."
#            ln -s /usr/X11R6/lib/libXpm.so.4 ${MAUS_ROOT_DIR}/third_party/install/lib/libXpm.so # known ROOT/SL4 bug since 2003 (!!)
#            extra="--with-xpm-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        fi

	rm -f ${MAUS_ROOT_DIR}/third_party/install/lib/libX11.so
	if [ -e "/usr/lib64/libX11.so.6" ] # sl55 64bit
        then
            echo "INFO: Applying SL5 64-bit libXpm fix since ROOT/SL suck."
            ln -s /usr/lib64/libX11.so.6 ${MAUS_ROOT_DIR}/third_party/install/lib/libX11.so # known ROOT/SL4 bug since 2003 (!!)
            extra="${extra} --with-x11-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        elif [ -e "/usr/lib/libX11.so.6" ] # sl55 32bit
        then
            echo "INFO: Applying SL5 32-bit libXpm fix since ROOT/SL suck."
            ln -s /usr/lib/libX11.so.6 ${MAUS_ROOT_DIR}/third_party/install/lib/libX11.so # known ROOT/SL4 bug since 2003 (!!)
            extra="${extra} --with-x11-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        elif [ -e "/usr/X11R6/lib64/libX11.so.6" ] # sl48 64bit
        then
            echo "INFO: Applying SL4 64-bit libXpm fix since ROOT/SL suck."
            #ln -s /usr/X11R6/lib64/libX11.so.6 ${MAUS_ROOT_DIR}/third_party/install/lib/libX11.so # known ROOT/SL4 bug since 2003 (!!)
            #extra="${extra} --with-x11-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        elif [ -e "/usr/X11R6/lib/libX11.so.6" ] # sl48 32bit
        then
            echo "INFO: Applying SL4 32-bit libXpm fix since ROOT/SL suck."
#            ln -s /usr/X11R6/lib/libX11.so.6 ${MAUS_ROOT_DIR}/third_party/install/lib/libX11.so # known ROOT/SL4 bug since 2003 (!!)
#            extra="${extra} --with-x11-libdir=${MAUS_ROOT_DIR}/third_party/install/lib"
        fi
		
	echo
        echo "INFO: Configuring:"
	echo
	sleep 1

	echo "INFO: extra flags to configure are ${extra}"
	./configure --enable-roofit --disable-fftw3 --disable-xrootd --disable-krb5 --build=debug --with-gsl-incdir=${MAUS_ROOT_DIR}/third_party/install/include --with-gsl-libdir=${MAUS_ROOT_DIR}/third_party/install/lib --prefix=${MAUS_ROOT_DIR}/third_party/install ${extra} && echo && echo && echo "INFO: Making:" && echo && sleep 1 && make && make install || { echo "FAIL: Failed to configure/make";exit 1; }

	            ################################################## 
	echo
        echo "INFO: The package should be locally build now in your"
	echo "INFO: third_party directory, which the rest of MAUS will"
	echo "INFO: find."
    else
	echo "FATAL: Source archive still doesn't exist.  Please file a bug report with your operating system,">&2
	echo "FATAL: distribution, and any other useful information at:" >&2
	echo "FATAL: " >&2
	echo "FATAL: http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/" >&2
	echo "FATAL:" >&2
	echo "FATAL: Giving up, sorry..." >&2
	exit 1
    fi
  
else         ################################################## 
echo
echo "FATAL: MAUS_ROOT_DIR is not set, which is required to" >&2  
echo "FATAL: know where to install this package.  You have two" >&2 
echo "FATAL: options:" >&2
echo "FATAL:" >&2
echo "FATAL: 1. Set the MAUS_ROOT_DIR from the command line by" >&2 
echo "FATAL: (if XXX is the directory where MAUS is installed):" >&2
echo "FATAL:" >&2 
echo "FATAL:        MAUS_ROOT_DIR=XXX ${0}" >&2
echo "FATAL:" >&2
echo "FATAL: 2. Run the './configure' script in the MAUS ROOT" >&2 
echo "FATAL: directory, run 'source env.sh' then rerun this" >&2  
echo "FATAL: command ">&2
exit 1
fi

# This is the maximum length of text (ignoring whitespace), 50 chars
##################################################
# this is the maximum amount of characters (including whitespace) per line)
######################################################################