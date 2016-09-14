#!/bin/bash
#

if [ -z $MAUS_ROOT_DIR ]
then
  echo "MAUS_ROOT_DIR is not set."
  echo "Please source a MAUS environment."
  echo
  exit 0
fi

WORK_DIR=${MAUS_ROOT_DIR}/bin/user/batch_cluster/
INPUT_DATA_DIR=${WORK_DIR}/output/analysis/
PLOT_DIR=${WORK_DIR}/output/analysis/plots/
PLOT_EXE=${WORK_DIR}/scripts/analysis/make_plots.py
TEST="OFF"

COMMAND="eval"
if [ "$TEST" == "ON" ]
then
  COMMAND="echo"
fi

DATA_FILES="validation_complete_straight
validation_full_straight
validation_complete_helix
validation_full_helix
resolution_helix_complete
resolution_helix_full
resolution_straight_complete
resolution_straight_full"

VALIDATION_FILES=$(ls $INPUT_DATA_DIR/validation_*.root)
RESOLUTION_FILES=$(ls $INPUT_DATA_DIR/resolution_*.root)


#for file in $VALIDATION_FILES ;
#do 
#  out_name=$(basename ${file%.*})
#  mkdir -p $PLOT_DIR/$out_name
#  python $MAUS_ROOT_DIR/My_MAUS/make_plots.py kalman_validation -D $PLOT_DIR/$out_name $file
#done

for file in $RESOLUTION_FILES ;
do 
  out_name=$(basename ${file%.*})
  mkdir -p $PLOT_DIR/$out_name
  python $PLOT_EXE resolution -D $PLOT_DIR/$out_name $file
done



