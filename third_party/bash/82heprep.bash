#!/usr/bin/env bash
writer_file=HepRepXMLWriter
url_writer=http://www.slac.stanford.edu/~perl/heprepxml/${writer_file}
url_viewer=http://www.slac.stanford.edu/~perl/HepRApp/HepRApp.jar
install_dir=${MAUS_ROOT_DIR}/third_party/install/heprep
here=`pwd`

echo
echo 'INFO: Installing HepRep files'
echo '------------------------------'
echo

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -d "$install_dir" ]; then
        rm -rf ${install_dir}
    fi
    mkdir ${install_dir}

    wget --directory-prefix=${install_dir} ${url_writer}.cc
    wget --directory-prefix=${install_dir} ${url_writer}.hh
    wget --directory-prefix=${install_dir} ${url_viewer}

    echo
    echo "INFO: Download complete"
    echo

    cd ${install_dir}

    echo
    echo "INFO: Updating the files with some sed commands"
    echo
    
    sed -i 's/fstream.h/fstream/' ${writer_file}.hh
    sed -i 's/ofstream/std::ofstream/' ${writer_file}.hh
    sed -i 's/fstream.h/iostream/' ${writer_file}.cc
    sed -i '21i #include <cstring>' ${writer_file}.cc
    sed -i '26i using namespace std;' ${writer_file}.cc
    
    echo
    echo "INFO: Installation complete"

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
