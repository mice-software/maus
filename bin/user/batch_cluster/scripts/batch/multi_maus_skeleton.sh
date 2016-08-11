#!/bin/sh
#
# multi_maus_skeleton.sh
#
# Script submitted to the Imperial batch system for multi_maus commands
#

################################################################################

#  THE DEFAULTS
# These are used as a last result if the relevant information cannot be found.

# The location of this version of MAUS.
MAUS_DIR=~/maus

# Local path to the simulate_mice file that calls Go()
# with respect to MAUS_ROOT_DIR
COMMAND=bin/simulate_mice.py

# At least the number of particles
REP_RATE="1000"

# Output filename
# The run number is appended to the prefix variable.
#  e.g. multi_maus_054.root
OUT_FILE_PREFIX=multi_maus_

# Environment Script
ENV_SCRIPT=env.sh

# Used Later...
FILENAME=""
FINAL_FILENAME=""

################################################################################



# Helper function for reporting errors
function error()
{
  echo -e "`echo -e $@ | sed 's/^/ERROR : /'`" >&2
  echo >&2
  exit 1
}



################################################################################




# If the job fails remove the output file to prevent errors in the analysis
function goingdown()
{
  echo
  echo
  echo "ERROR"
  echo
  echo "Caught $1"
  echo "Killing Process $2"
  echo
  kill -KILL $2
  echo "Removing the output file, $FILENAME"
  echo
  rm $FILENAME
  echo "Ciao..."
  echo
}



################################################################################


# Makes a trap for each signal - so that we know what signal we just caught!
function trap_with_arg()
{
  pid=$1 ; shift
  func=$1 ; shift
  for sig
  do
    trap "$func $sig $pid" "$sig"
  done
}



################################################################################


if [ -n "$MULTI_MAUS_RANDOMWAIT" ]
then
  number=$RANDOM
  let "number %= $MULTI_MAUS_RANDOMWAIT"
  echo "Waiting for $number seconds before starting."
  echo "This offsets the start and end of bash jobs to reduce the number of synchronous read/write commands."
  sleep ${number}s
fi


if [ -z "$MULTI_MAUS_MAUSDIR" ]
then
  error "Did not find the MAUS Directory"
fi
MAUS_DIR=$MULTI_MAUS_MAUSDIR


if [ -z "$MULTI_MAUS_COMMAND" ]
then
  error "Did not find the MAUS Running Command"
fi
COMMAND=$MULTI_MAUS_COMMAND


if [ -z "$MULTI_MAUS_CONFIG" ]
then
  error "Did not find a configuration file"
elif ! $( echo $MULTI_MAUS_CONFIG | egrep -q '.*\/Conf_.*\.py' )
then
  error "Not a valid config file"
fi
CONFIG_FILE="$MULTI_MAUS_CONFIG"

if [ -z "$MULTI_MAUS_DATADIR" ]
then
  error "Did not find an output data directory"
fi
BASE=$( echo $CONFIG_FILE | sed 's/.*\/Conf_\(.*\)\.py/\1/' )
OUT_DIR=$MULTI_MAUS_DATADIR/$BASE
TMP_DIR=$TMPDIR/$BASE
mkdir -p $TMP_DIR

if [ -n "$MULTI_MAUS_DATA_SUBDIR" ]
then
  OUT_DIR=$OUT_DIR/$MULTI_MAUS_DATA_SUBDIR
  TMP_DIR=$TMP_DIR/$MULTI_MAUS_DATA_SUBDIR
fi

#if [ -z "$MULTI_MAUS_DAQ_FILE" ]
#then
#  error "Did not find a DAQ File"
#fi
DAQ_FILE=$MULTI_MAUS_DAQ_FILE


################################################################################

OPTIONS=""

echo "MAUS is located : $MAUS_DIR"

if [ ! -d $MAUS_DIR ]
then
  error "Could not find MAUS Root Directory\nDid you remember to source \"env.sh\""
fi

cd $MAUS_DIR


if [ $? -ne 0 ] 
then
  error "Could not 'cd' into MAUS Root Directory"
fi


if [ ! -f $ENV_SCRIPT ]
then
  error "Could not find environment script to source"
fi

source $ENV_SCRIPT

if [ ! -f "$CONFIG_FILE" ]
then
  error "Could not find config file"
fi

if [ -n "$DAQ_FILE" ]
then
  echo "Using DAQ File $DAQ_FILE"
  OPTIONS=" --daq_data_file $DAQ_FILE"
fi

OPTIONS="$OPTIONS --configuration_file $CONFIG_FILE"

if [ ! -d "$OUT_DIR" ]
then
  mkdir -p $OUT_DIR
fi

cp $CONFIG_FILE $OUT_DIR/$( basename $CONFIG_FILE )

if [ $? -ne 0 ]
then
  error "Could not write to output directory"
fi

echo 
echo "You Are Here:"
pwd
echo

echo "Data Output Goes Here:"
echo $TMP_DIR

echo "And is copied to here:"
echo $OUT_DIR

if [ ! -f "$COMMAND" ]
then
  error "Could not find MAUS Running command, $COMMAND"
fi

NUM=""
if [ -n "$DAQ_FILE" ]
then
  NUM=$DAQ_FILE
else
  NUM=`printf "%03d" $SGE_TASK_ID`
fi
FILENAME="$TMP_DIR/${OUT_FILE_PREFIX}${NUM}.root"
FINAL_FILENAME="$OUT_DIR/${OUT_FILE_PREFIX}${NUM}.root"
SEED=`echo "($SGE_TASK_ID * $REP_RATE)/1" | bc`


echo
echo "Creating File $FILENAME"
echo "SEED = $SEED"
echo

## Export all MAUS Variables:
MAUS_RANDOM_SEED=$SEED

all_variables=$( ( set -o posix ; set ) ) ; IFS=$'\n' ; set -f
for var in $all_variables
do 
  if [[ $var == MAUS_* ]] 
  then
    echo "export $var"
    eval export $var
  fi
done
IFS=$' '
set +f


# If the Global Random Seed is functional in MAUS then use this one.
python $COMMAND $OPTIONS --output_root_file_name $FILENAME &

pid=$!

trap_with_arg $pid goingdown SIGINT SIGTERM SIGUSR1 SIGUSR2 SIGXCPU

wait $pid

echo
echo "Copying File to Final Location"
echo
cp $FILENAME $FINAL_FILENAME && rm $FILENAME

echo
echo "Completed!"
echo

