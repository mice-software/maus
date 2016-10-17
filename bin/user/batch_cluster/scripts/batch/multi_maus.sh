#!/bin/sh
#
# multi_maus.sh
#
# Running script to automate the multi_maus submission process
#


# USER OPTIONS
################################################################################

MAUS_DIR=${MAUS_ROOT_DIR}

WORK_DIR="${MAUS_ROOT_DIR}/bin/user/batch_cluster"

MULTI_MAUS="${WORK_DIR}/scripts/batch/multi_maus_skeleton.sh"

DATA_DIR="${WORK_DIR}/data"

COMMAND="bin/user/simulate_trackervirtuals.py"

################################################################################


# DEFAULT OPTIONS
################################################################################

NUMBER_TASKS=100

ADDITIONAL_DEFINES=""

DATA_SUBDIR=""

OUTPUT_DIR="${WORK_DIR}/output/batch_messages/"

MULTI_MAUS_CONFIG=""

OPTIONS=""

RANDOM_WAIT_LENGTH=300

MULTI_MAUS_RANDOMWAIT=""

################################################################################

if [ -z $MAUS_DIR ]
then
  echo "MAUS_ROOT_DIR not set."
  echo "Please source the environment script first."
  exit 0
fi


################################################################################
  ##  FUNCTIONS

function print_help() {
  echo
  echo "Usage : "
  echo 
  echo "multi_maus.sh <QUEUE-LENGTH> <CONFIGURATION-FILE>  [ <OPTIONS> ]"
  echo
  echo "QUEUE-LENGTH must be one of \"short\", \"medium\" or \"long\"."
  echo
  echo "Options may be any of:"
  echo 
  echo " -N  <N>    Number of Jobs"
  echo " -C  <C>    Running Command"
  echo " -O  <O>    Output Data Directory"
  echo "            (Don't use default directory name)"
  echo " -S  <S>    Output Data Subdirectory"
  echo " -D  <D=V>   Environment variable definition"
  echo " -W  <M>    Wait before starting job."
  echo "            Only M jobs will run in parallel."
  echo
  exit 0
}


################################################################################


if (( $# )) 
then

  if [ "$1" == "-h" -o "$1" == "--help" ]
  then 
    print_help
  fi


  QUEUE_LENGTH=$1
  if [ "$QUEUE_LENGTH" != "short" ] && [ "$QUEUE_LENGTH" != "medium" ] && [ "$QUEUE_LENGTH" != "long" ]
  then
    echo "ERROR : First argument must be the chosen queue length."
    echo "Please select from \"short\", \"medium\" or \"long\""
    exit 0
  fi
  shift
else
  print_help
fi

if (( $# ))
then
  MULTI_MAUS_CONFIG=$1
  if [ ! -f $MULTI_MAUS_CONFIG ]
  then
    echo 
    echo "ERROR : Coud not find configuration file " $MULTI_MAUS_CONFIG
    echo
    exit 0
  fi
  shift
else
  echo
  echo "ERROR : Please provide the name of the configuration file"
  echo
  exit 0
fi

while [[ $# > 0 ]]
do
  key=$1

  case $key in 
    -O)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-O'"
      exit 0
    fi
    DATA_DIR=$val
    echo "Set data output directory to $DATA_DIR"
    shift
    shift
    ;;

    -S)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-S'"
      exit 0
    fi
    DATA_SUBDIR=$val
    echo "Set data subdirectory to $DATA_SUBDIR"
    shift
    shift
    ;;

    -D)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-D'"
      exit 0
    fi
    if ! $( echo $val | egrep -q '^.*=.*$' )
    then
      echo
      echo "Invalid Variable Definition: $val"
      echo
    fi
    eval export $val
    variable_name=$(echo $val | sed 's/\(.*\)=.*/\1/g' )
    ADDITIONAL_DEFINES="$ADDITIONAL_DEFINES -v $variable_name"
    echo "Defined $val"
    shift
    shift
    ;;

    -N)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-N'"
      exit 0
    fi
    if ! $(echo $val | egrep -q '^[0-9]*$')
    then
      echo
      echo "ERROR : Number of jobs must be a positive integer"
      echo
      exit 0
    fi
    NUMBER_TASKS=$val
    echo "Set number of tasks to $NUMBER_TASKS"
    shift
    shift
    ;;

    -C)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-C'"
      exit 0
    fi
    if [ ! -f $val ]
    then
      echo
      echo "ERROR : Could not find running command, $val"
      echo
      exit 0
    fi
    COMMAND=$val
    echo "Set command to $COMMAND"
    shift
    shift
    ;;

    -W)
    val=$2
    if [[ $val = -* ]]
    then
      echo "ERROR : No argument supplied after option, '-C'"
      exit 0
    fi
    if ! $(echo $val | egrep -q '^[0-9]*$')
    then
      echo
      echo "ERROR : Number of parallel jobs must be a positive integer"
      echo
      exit 0
    fi
    echo "Jobs will wait for a random time period before start."
    echo "Between 0 and $RANDOM_WAIT_LENGTH seconds."
    echo "Only $val jobs will be run in parallel."
    MULTI_MAUS_RANDOMWAIT=$RANDOM_WAIT_LENGTH
    OPTIONS="$OPTIONS -tc $val"
    shift
    shift
    ;;

    *)
    echo
    echo "Warning! Unrecognised Option: $key"
    echo
    shift
    ;;
  esac

done

if [ ! -d $OUTPUT_DIR ]
then
  mkdir -p $OUTPUT_DIR
fi

TIME=""
if [ "$QUEUE_LENGTH" == "short" ]
then 
  TIME="3:0:0"
elif [ "$QUEUE_LENGTH" == "medium" ]
then
  TIME="6:0:0"
else
  TIME="48:0:0"
fi
LENGTH="-l h_rt=$TIME"

OPTIONS="$OPTIONS $LENGTH -q hep.q -v MULTI_MAUS_MAUSDIR -v MULTI_MAUS_CONFIG -v MULTI_MAUS_COMMAND -v MULTI_MAUS_DATADIR -v MULTI_MAUS_DATA_SUBDIR -v MULTI_MAUS_RANDOMWAIT -j y -o $OUTPUT_DIR -t 001-$NUMBER_TASKS:1 $ADDITIONAL_DEFINES"

echo
echo "Using configuration file " $MULTI_MAUS_CONFIG
echo "Launching" $NUMBER_TASKS "Jobs to the" $QUEUE "queue"
echo
echo "Using the following command:"
echo
echo "qsub $OPTIONS"
echo

export MULTI_MAUS_MAUSDIR=$MAUS_DIR
export MULTI_MAUS_CONFIG
export MULTI_MAUS_RANDOMWAIT
export MULTI_MAUS_COMMAND=$COMMAND
export MULTI_MAUS_DATADIR=$DATA_DIR
export MULTI_MAUS_DATA_SUBDIR=$DATA_SUBDIR

#echo "qsub $OPTIONS $MULTI_MAUS"
qsub $OPTIONS $MULTI_MAUS
echo
echo


################################################################################


