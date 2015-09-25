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


while [[ $# > 1 ]]
do
key="$1"

case $key in
    -j|--num-threads)
    if expr "$2" : '-\?[0-9]\+$' >/dev/null
    then
        MAUS_NUM_THREADS="$2"
    fi
    shift
    ;;
    -t|--third-party-dir)
    MAUS_THIRD_PARTY="$2"
    shift
    ;;
    -u|--unpacker)
    MAUS_UNPACKER_VERSION="$2"
    shift
    ;;
    -v|--verbosity)
    if [ $2 -eq 0 ] || [ $2 -eq 1 ]; then
      MAUS_BUILD_VERBOSITY="$2"
    fi
    shift
    ;;
    -g|--use-system-gcc)
    if [ $2 -eq 0 ] || [ $2 -eq 1 ]; then
    USE_SYSTEM_GCC="$2"
    fi
    shift
    ;;
esac
shift
done

if [ -z "$USE_SYSTEM_GCC" ]; then
  USE_SYSTEM_GCC=0
fi
if [ "$USE_SYSTEM_GCC" -eq 0 ]; then
  echo "Using system GCC"
else
  echo "Will build GCC as third party"
fi

if [ -z "$MAUS_NUM_THREADS" ]; then
  MAUS_NUM_THREADS=1
fi
echo "Number of threads to build with:" "${MAUS_NUM_THREADS}"
echo

if [ "$MAUS_THIRD_PARTY" ]; then
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

if [ -z "$MAUS_UNPACKER_VERSION" ]; then
  MAUS_UNPACKER_VERSION="StepIV"
fi
if [ "$MAUS_UNPACKER_VERSION" = "StepI" ] || [ "$MAUS_UNPACKER_VERSION" = "StepIV" ];  then
  echo "MAUS Unpacker version to use:" "${MAUS_UNPACKER_VERSION}"
  echo
else
  echo "FATAL: Bad MAUS_UNPACKER_VERSION supplied"
  echo "FATAL: Please use either StepI or StepIV or leave unset"
  exit 1
fi

if [ -z "$MAUS_BUILD_VERBOSITY" ]; then
  MAUS_BUILD_VERBOSITY=0
fi

uname -a 2>>$FILE_STD 1>>$FILE_STD 
echo "Configuring..."
if [ "$MAUS_THIRD_PARTY" ]; then
    ./configure -t $MAUS_THIRD_PARTY -u $MAUS_UNPACKER_VERSION 2>> $FILE_STD 1>> $FILE_STD
    echo "Sourcing the environment..."
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
    echo "Installing field maps in MAUS_ROOT_DIR..."
    if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
        ./third_party/bash/45beamline_fieldmaps.bash 2>>$FILE_STD 1>>$FILE_STD
    else
        ./third_party/bash/45beamline_fieldmaps.bash 2>&1 | tee -a $FILE_STD
    fi
    # Check new ROOT version alone is used
    source ${MAUS_THIRD_PARTY}/third_party/build/root/bin/thisroot.sh
else
    if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
        ./configure -u $MAUS_UNPACKER_VERSION 2>>$FILE_STD 1>>$FILE_STD
    else
        ./configure -u $MAUS_UNPACKER_VERSION 2>&1 | tee -a $FILE_STD
    fi
    echo "Sourcing the environment..."
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
    echo "Building third party libraries (takes a while...)"
    if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
        ./third_party/build_all.bash -j $MAUS_NUM_THREADS -g $USE_SYSTEM_GCC 2>>$FILE_STD 1>>$FILE_STD
    else
        ./third_party/build_all.bash -j $MAUS_NUM_THREADS -g $USE_SYSTEM_GCC 2>&1 | tee -a $FILE_STD
    fi
    echo "Resource the environment (catches the new ROOT version)"
    source env.sh 2>>$FILE_STD 1>>$FILE_STD
    # Check new ROOT version alone is used
    source ${MAUS_ROOT_DIR}/third_party/build/root/bin/thisroot.sh
fi

if [ -z "$MAUS_THIRD_PARTY" ]; then
    export MAUS_THIRD_PARTY=$MAUS_ROOT_DIR
fi

# Check new ROOT version alone is used
source ${MAUS_THIRD_PARTY}/third_party/build/root/bin/thisroot.sh

echo "Cleaning the MAUS build state"
if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
    scons -c 2>>$FILE_STD 1>>$FILE_STD
else
    scons -c 2>&1 | tee -a $FILE_STD
fi

echo "Building MAUS"
if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
    (scons build -j${MAUS_NUM_THREADS} || (echo "FAIL! See logs.x" && exit 1))  2>>$FILE_STD 1>>$FILE_STD
else
    (scons build -j${MAUS_NUM_THREADS} || (echo "FAIL! See logs.x" && exit 1))  2>&1 | tee -a $FILE_STD
fi
if [ $? != 0 ]; then
    cat $FILE_STD
    echo "FAIL Failed to make MAUS using scons. Fatal error - aborting"
    exit 1
fi

echo "Run the tests"
if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
    bash ${MAUS_ROOT_DIR}/tests/unit_tests.bash  2>>$FILE_STD 1>>$FILE_STD
else
    bash ${MAUS_ROOT_DIR}/tests/unit_tests.bash  2>&1 | tee -a $FILE_STD
fi
if [ $? != 0 ]
then
    cat $FILE_STD
    echo "FAIL Failed unit tests. Fatal error - aborting"
    exit 1
fi

if [ $MAUS_BUILD_VERBOSITY -eq 0 ]; then
    bash ${MAUS_ROOT_DIR}/tests/style_tests.bash  2>>$FILE_STD 1>>$FILE_STD
else
    bash ${MAUS_ROOT_DIR}/tests/style_tests.bash  2>&1 | tee -a $FILE_STD
fi
if [ $? != 0 ]
then
    cat $FILE_STD
    echo "FAIL Failed style tests"
    exit 1
fi

