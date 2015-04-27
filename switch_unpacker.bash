#!/bin/bash

if [ -z "$MAUS_ROOT_DIR" ]; then
   echo "FATAL: \$MAUS_ROOT_DIR not set. Please source env.sh"
   exit 1;
fi
if [ -z "$MAUS_THIRD_PARTY" ]; then
   echo "FATAL: \$MAUS_THIRD_PARTY not set. Please source env.sh"
   exit 1;
fi

if [ -z "$1" ]; then
   echo "FATAL: No argument supplied. Please supply either StepI or StepIV"
   exit 1;
fi

if [ ! "$MAUS_ROOT_DIR" = "$MAUS_THIRD_PARTY" ]; then
   echo "FATAL: MAUS_ROOT_DIR != MAUS_THIRD_PARTY - rebuild the unpacker manually"
   exit 1;
fi

echo "INFO: Current unpacker version is set as: " $MAUS_UNPACKER_VERSION
echo "INFO: Current unpacker is located in: " $MAUS_THIRD_PARTY

if [ "$1" = "StepI" ]; then
  echo "INFO: Setting up StepI unpacker"
  export MAUS_UNPACKER_VERSION="StepI"
  sed -i '/MAUS_UNPACKER_VERSION/c\     export MAUS_UNPACKER_VERSION="StepI"' env.sh
elif [ "$1" = "StepIV" ]; then
  echo "INFO: Setting up StepIV unpacker"
  sed -i '/MAUS_UNPACKER_VERSION/c\     export MAUS_UNPACKER_VERSION="StepIV"' env.sh
  export MAUS_UNPACKER_VERSION="StepIV"
else
  echo "FATAL: Bad argument supplied. Please supply either StepI or StepIV"
  exit 1;
fi

echo
echo "INFO: Rebuilding the unpacker"
echo

if $MAUS_THIRD_PARTY/third_party/bash/53unpacking.bash; then
  echo
  echo "INFO: Unpacker rebuilt"
  echo
else
  echo
  echo "FATAL: Building the unpacker failed"
  echo
  exit 1;
fi

echo
echo "INFO: Rebuilding MAUS"
echo
if scons -c; then
  echo
  echo "INFO: MAUS build cleaned"
  echo
else
  echo
  echo "FATAL: Cleaning the build failed"
  echo
  exit 1;
fi
if scons -j8; then
  echo
  echo "INFO: MAUS build succeeded"
  echo
else
  echo
  echo "FATAL: Failed to build MAUS"
  echo
  exit 1;
fi

echo
echo "INFO: Testing the unpacker"
echo

if python src/input/InputCppDAQOfflineData/test_InputCppDAQOfflineData.py; then
  echo
  echo "INFO: Unpacker changed successfully"
  echo
else
  echo
  echo "FATAL: Unpacker test failed"
  echo
  exit 1;
fi
