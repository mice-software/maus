#!/usr/bin/env bash
#http://www.linuxfromscratch.org/blfs/view/6.3/general/libxslt.html

#http://bazaar.launchpad.net/~antony-wilson/mcdb/mice.cdb.client.api-python/download/head:/cdb1.0.0.0.tar.gz-20130212090645-5dg2axdjmtby60vz-1/cdb-1.0.0.0.tar.gz

version=1.1.3
unique_id=20150903113311-myc75whjxkh6manr-1
directory=cdb-${version}
filename=cdb-${version}.tar.gz
filename_mash=cdb${version}.tar.gz-${unique_id}/${filename}
# this url does not seem to work anymore - what? when?
# url=http://bazaar.launchpad.net/~antony-wilson/mcdb/mice.cdb.client.api-python/download/head:/${filename_mash}
#url=http://micewww.pp.rl.ac.uk/attachments/download/2180/cdb-${version}.tar.gz
# as of version 1.1.2 the tarballs are in Janusz's branch
url=http://bazaar.launchpad.net/~janusz-martyniak/mcdb/mice.cdb.client.api-python/download/head:/${filename_mash}

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
        cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
        echo
        echo "INFO: Installing:"
        echo
        sleep 1
        python setup.py install
        echo 'import cdb' | python
        if [ $? != 0 ]; then
            echo 
            echo "FATAL: The package failed to build. Review the build output"
            echo "FATAL: and consider submitting a support request at"
            echo "FATAL: micewww.pp.rl.ac.uk/projects/maus/issues/"
            echo
            exit 1
        fi

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
