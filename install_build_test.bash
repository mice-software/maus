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

echo "   Welcome to the all-in-one MAUS installer script.  This script will"
echo "take about 1 hour and 7 minutes to run.  You can get the details of the"
echo "progress (or ensure it is doing something) by running:"
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

echo "Configuring..."
./configure 2>>$FILE_ERR 1>>$FILE_STD

echo "Sourcing the environment..."
source env.sh 2>>$FILE_ERR 1>>$FILE_STD

echo "Building third party libraries (takes a while...)"
./third_party/build_all.bash 2>>$FILE_ERR 1>>$FILE_STD

echo "Resource the environment (catches the new ROOT version)"
source env.sh 2>>$FILE_ERR 1>>$FILE_STD

echo "Have Scons cleanup the MAUS build state"
scons -c 2>>$FILE_ERR 1>>$FILE_STD

echo "Build MAUS"
scons build || (echo "FAIL! See logs.x" && exit 1)  2>>$FILE_ERR 1>>$FILE_STD

echo "Run the tests"
./tests/run_tests.bash || (echo "FAIL!  See logs." && exit 1) 2>>$FILE_ERR 1>>$FILE_STD

