#!/bin/bash

FILE_STD=install.log

if [ ! -z "$MAUS_ROOT_DIR" ]; then
   echo "FATAL: Trying to build with \$MAUS_ROOT_DIR set. Please start a new"
   echo "FATAL: session (and don't source an existing MAUS installation)"
   echo "FATAL: before attempting to build"
   exit 1;
fi

if [ -f $FILE_STD ];
then
    rm $FILE_STD
fi

echo
echo "   Welcome to the all-in-one MAUS installer script. "
echo "You can get the details of the progress "
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
echo

# Assign the location of the third party libraries  
# In order of preference the location is set to:
# 1. The first command line argument passed to the install script
# 2. Any existing environment variable called "maus_third_party" e.g. if set by user's .bashrc file
# 3. The current maus working directory, as held by the variable MAUS_ROOT_DIR
# If not location is found then the script aborts 
cmake --version
if [ $? != 0 ]; then
    echo "FATAL: Require cmake to build g4.9.6.p02"
    echo "FATAL: Exiting. Install cmake and try again"
    exit 1
fi
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

uname -a 2>>$FILE_STD 1>>$FILE_STD 
echo "Configuring..."
if [ "$MAUS_THIRD_PARTY" ]; then
    ./configure -t $MAUS_THIRD_PARTY 2>> $FILE_STD 1>> $FILE_STD
    echo "Sourcing the environment..."
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
    echo "Installing field maps in MAUS_ROOT_DIR..."
    ./third_party/bash/45beamline_fieldmaps.bash 2>>$FILE_STD 1>>$FILE_STD
else
    echo "The other loop"
    ./configure 2>>$FILE_STD 1>>$FILE_STD
    echo "Sourcing the environment..."
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
    echo "Building third party libraries (takes a while...)"
    ./third_party/build_all.bash 2>>$FILE_STD 1>>$FILE_STD
    echo "Resource the environment (catches the new ROOT version)"
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
        #./third_party/bash third_party/bash/29expat.bash
        #./third_party/bash third_party/bash/32clhep2.1.1.0.bash
        #./third_party/bash third_party/bash/35geant4.9.6.bash
        # source env_geant4.9.6.p02.sh 2>>$FILE_STD 1>>$FILE_STD
fi

echo "Cleaning the MAUS build state"
scons -c 2>>$FILE_STD 1>>$FILE_STD

echo "Building MAUS"
(scons build || (echo "FAIL! See logs.x" && exit 1))  2>>$FILE_STD 1>>$FILE_STD
if [ $? != 0 ]; then
    cat $FILE_STD
    echo "FAIL Failed to make MAUS using scons. Fatal error - aborting"
    exit 1
fi

echo "Run the tests"
bash ${MAUS_ROOT_DIR}/tests/unit_tests.bash  2>>$FILE_STD 1>>$FILE_STD
if [ $? != 0 ]
then
    cat $FILE_STD
    echo "FAIL Failed unit tests. Fatal error - aborting"
    exit 1
fi

bash ${MAUS_ROOT_DIR}/tests/style_tests.bash  2>>$FILE_STD 1>>$FILE_STD
if [ $? != 0 ]
then
    cat $FILE_STD
    echo "FAIL Failed style tests"
    exit 1
fi

