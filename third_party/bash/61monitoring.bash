directory=monitor
filename=lib${directory}.tarz
url=http://micewww.pp.rl.ac.uk/maus/MAUS_release_version_1.1.0/${filename}

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    cd ${MAUS_ROOT_DIR}/third_party/source

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename} ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}
    fi

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 ]
    then
      rm ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5
    fi

    wget ${url}
    wget ${url}.md5

    if [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename} ] &&
       [ -f ${MAUS_ROOT_DIR}/third_party/source/${filename}.md5 ]
    then
        echo "INFO: Source archive file is download."
        echo
        echo "INFO: Checking MD5 checksum (otherwise the file didn't"
        echo "INFO: download properly):"
        echo

        md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
        sleep 1
        echo
        echo "INFO: Monitoring:"
        echo

        mkdir montmp
        tar xvfz ${filename} -C montmp

        if [ ! -d "${MAUS_ROOT_DIR}/third_party/install/include/${directory}" ]; then
          mkdir ${MAUS_ROOT_DIR}/third_party/install/include/${directory}
        fi

        cp montmp/*.h* ${MAUS_ROOT_DIR}/third_party/install/include/${directory}/
        cp montmp/*.a  ${MAUS_ROOT_DIR}/third_party/install/lib/
        rm -rf montmp
    else
        echo "FATAL: Unable to download the source archive files. Please file a bug report at:" >&2
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
