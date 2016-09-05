#!/bin/bash
#

# This scripts analyses the data from everything in this directory
# See the make_plots script for spitting out the pretty pdfs.

if [ -z $MAUS_ROOT_DIR ]
then
  echo "MAUS_ROOT_DIR is not set."
  echo "Please source a MAUS environment."
  echo
  exit 0
fi

WORK_DIR=${MAUS_ROOT_DIR}/bin/user/batch_cluster/
OUTPUT_DIR=${WORK_DIR}/output/analysis/
INPUT_DATA_DIR=${WORK_DIR}/output/simulation/
PY_EXE=${MAUS_THIRD_PARTY}/third_party/install/bin/python
QUEUE="hep.q"
DURATION="5:50:00"
TEST="OFF"
BINS_AND_WINDOWS="--pt_window 0.0 100.0 --pt_bin 10 --pz_bin 10 --pz_window 140.0 250.0" #--max_gradient 0.6"
GRADIENT_SELECTION="--max_gradient 0.5"
RESIDUALS="" #"-R" # Weights them wrt errors
KALMAN_VALIDATION_SCRIPT=${WORK_DIR}/scripts/analysis/kalman_validation.py
COMMAND="eval"
if [ "$TEST" == "ON" ]
then
  COMMAND="echo"
fi


echo "Analysing Straight Data"
echo

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $KALMAN_VALIDATION_SCRIPT $RESIDUALS --track_algorithm 0 -O validation_full_straight -D $OUTPUT_DIR/ $INPUT_DATA_DIR/multi_straight_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $KALMAN_VALIDATION_SCRIPT $RESIDUALS --track_algorithm 0 -O validation_complete_straight -D $OUTPUT_DIR/ $INPUT_DATA_DIR/multi_straight_complete/multi_maus_*.root

echo "Analysing Helix Data"
echo

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $KALMAN_VALIDATION_SCRIPT $RESIDUALS --track_algorithm 1 -O validation_full_helix -D $OUTPUT_DIR/ $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $KALMAN_VALIDATION_SCRIPT $RESIDUALS --track_algorithm 1 -O validation_complete_helix -D $OUTPUT_DIR/ $INPUT_DATA_DIR/multi_helix_complete/multi_maus_*.root


echo "Analysing Resolutions"
echo

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --not_require_cluster $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_helix_full $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --not_require_cluster $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_helix_complete $INPUT_DATA_DIR/multi_helix_complete/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py --not_require_cluster --track_algorithm 0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_straight_full $INPUT_DATA_DIR/multi_straight_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py --not_require_cluster --track_algorithm 0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_straight_complete $INPUT_DATA_DIR/multi_straight_complete/multi_maus_*.root


qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_complete $INPUT_DATA_DIR/multi_helix_complete/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py --track_algorithm 0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_straight_full $INPUT_DATA_DIR/multi_straight_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py --track_algorithm 0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_straight_complete $INPUT_DATA_DIR/multi_straight_complete/multi_maus_*.root


echo "Momentum Window Analyses"
echo

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --p_window 190.0 200.0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full_190-200 $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --p_window 200.0 210.0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full_200-210 $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --p_window 185.0 215.0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full_200 $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --p_window 125.0 155.0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full_140 $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root

qsub -q $QUEUE -l h_rt=${DURATION} -V -S ${PY_EXE} $MAUS_ROOT_DIR/bin/user/scifi/tracker_resolution_plots.py -C --p_window 225.0 255.0 $GRADIENT_SELECTION $BINS_AND_WINDOWS --cut_number_trackpoints 0 -D $OUTPUT_DIR/ -O resolution_reqCluster_helix_full_240 $INPUT_DATA_DIR/multi_helix_full/multi_maus_*.root
