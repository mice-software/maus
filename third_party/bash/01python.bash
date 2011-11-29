#!/usr/bin/env bash
version=2.7.2
directory=Python-${version}
filename=${directory}.tgz
url=http://www.python.org/ftp/python/${version}/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."

	wget --directory-prefix="${MAUS_ROOT_DIR}/third_party/source" ${url}
    fi
	
    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "INFO: Source archive exists."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo
        cd "${MAUS_ROOT_DIR}/third_party/source"
        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Unpacking:"
        echo
	rm -Rf "${MAUS_ROOT_DIR}/third_party/build/${directory}"
        sleep 1
        tar xvfz "${MAUS_ROOT_DIR}/third_party/source/${filename}" -C "${MAUS_ROOT_DIR}/third_party/build" > /dev/null
        cd "${MAUS_ROOT_DIR}/third_party/build/${directory}"

	echo "INFO: Configuring:"
        sleep 1
	if [ `uname -s` == "Darwin" ]; then
            ./configure --enable-framework="${MAUS_ROOT_DIR}/third_party/install" --prefix="${MAUS_ROOT_DIR}/third_party/install"
	else
            ./configure --enable-shared --prefix="${MAUS_ROOT_DIR}/third_party/install"
	fi

	echo "HACK: Enabling zlib (again?)"
        sleep 1

	if [ "$(uname -m)" = "x86_64" ]; then
	    echo "     This is an x86_64 Linux"
	    echo "zlib zlibmodule.c -I/usr/include -L/usr/lib64 -lz" >> Modules/Setup

	    # See issue #774
	    echo "_sha shamodule.c" >> Modules/Setup
            echo "_sha256 sha256module.c" >> Modules/Setup
	    echo "_sha512 sha512module.c" >> Modules/Setup
	elif [ `uname -s` != "Darwin" ]; then
	    echo "     This is NOT an x86_64 Linux"
	    echo "zlib zlibmodule.c -I/usr/include -L/usr/lib -lz" >> Modules/Setup
	fi

        echo "INFO: Making:"
        sleep 1
        make
        echo "INFO: Installing within MAUS's third party directory:"
        if [ `uname -s` == "Darwin" ]; then
          make frameworkinstallstructure \
               altinstall \
               bininstall \
               maninstall \
               frameworkinstallmaclib \
               frameworkinstallunixtools
        else
          make install
        fi
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
else
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
