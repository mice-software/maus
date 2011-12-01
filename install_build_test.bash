#!/bin/bash

FILE_STD=install_log_std

if [ -f $FILE_STD ];
then
    rm $FILE_STD
fi

FILE_ERR=install_log_err

if [ -f $FILE_ERR ];
then
    rm $FILE_ERR
fi

if [ "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet                                                                                                                
    echo "Your current directory is:"
    pwd
    echo
    echo "Your MAUS_ROOT_DIR is:"
    echo ${MAUS_ROOT_DIR}
    echo
    echo "These should agree"
fi

echo
echo "   Welcome to the MAUS installer script.  This script will build MAUS using"
echo "precompiled third party libraries. You can get the details of the progress "
echo "(or ensure it is doing something) by running:"
echo
echo "  tail -f $FILE_STD"
echo
echo "and if you run into a problem with the installation, then please please"
echo "please post your error here:"
echo
echo "   http://micewww.pp.rl.ac.uk/projects/maus/issues/new"
echo
echo "so we can build up a database of errors people have seen and how they"
echo "solved them.  Be sure to attach the files:"
echo
echo "   $FILE_STD"
echo "   $FILE_ERR"
echo

# Assign the location of the third party libraries  
# In order of preference the location is set to:
# 1. The first command line argument passed to the install script
# 2. Any existing environment variable called "maus_third_party" e.g. if set by user's .bashrc file
# 3. The current maus working directory, as held by the variable MAUS_ROOT_DIR
# If not location is found then the script aborts 
if [ "$1" ]; then
    MAUS_THIRD_PARTY=$1
    echo "Your MAUS_THIRD_PARTY is:"
    echo ${MAUS_THIRD_PARTY}
    echo
elif [ "${maus_third_party}" ]; then
    MAUS_THIRD_PARTY=${maus_third_party}
    echo "Your MAUS_THIRD_PARTY is:"
    echo ${MAUS_THIRD_PARTY}
    echo
else
    echo "No MAUS_THIRD_PARTY set, installing third party libraries locally"
    echo
fi

echo "Configuring..."
if [ "MAUS_THIRD_PARTY" ]; then
	./configure $MAUS_THIRD_PARTY 2>>$FILE_ERR 1>>$FILE_STD
else
	./configure 2>>$FILE_ERR 1>>$FILE_STD
fi

echo "Sourcing the environment..."
source env.sh 2>>$FILE_ERR 1>>$FILE_STD

echo "Have Scons cleanup the MAUS build state"
scons -c 2>>$FILE_ERR 1>>$FILE_STD

echo "Build MAUS"
scons build || (echo "FAIL! See logs.x" && exit 1)  2>>$FILE_ERR 1>>$FILE_STD

echo "Run the tests"
./tests/run_tests.bash || (echo "FAIL!  See logs." && exit 1) 2>>$FILE_ERR 1>>$FILE_STD

