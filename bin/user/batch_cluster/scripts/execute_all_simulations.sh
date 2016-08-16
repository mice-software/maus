#!/bin/bash

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

# Run all the simulations to generate tracker performance data

if [ -z $MAUS_ROOT_DIR ]
then
  echo "MAUS_ROOT_DIR is not set."
  echo "Please source a MAUS environment."
  echo
  exit 0
fi

NUMBER_JOBS=100
MAX_PARALLEL=100
DURATION="short"
WORK_DIR=${MAUS_ROOT_DIR}/bin/user/batch_cluster/
DATA_OUT=${WORK_DIR}/output/simulation/
MULTI_MAUS_SCRIPT=${WORK_DIR}/scripts/batch/multi_maus.sh
SIMULATE_VIRTUALS_EXE=${WORK_DIR}/scripts/simulation/simulate_tracker_virtuals.py
DATACARD_DIR=${WORK_DIR}/datacards/
SIMULATE_STANDARD_EXE=${WORK_DIR}/scripts/simulation/simulate_standard.py
TEST="OFF"

COMMAND="eval"
if [ "$TEST" == "ON" ]
then
  COMMAND="echo"
fi

echo "Complete MC"

$COMMAND $MULTI_MAUS_SCRIPT $DURATION ${DATACARD_DIR}/Conf_multi_straight_full.py -W $MAX_PARALLEL -C $SIMULATE_VIRTUALS_EXE -O $DATA_OUT -N $NUMBER_JOBS

$COMMAND $MULTI_MAUS_SCRIPT $DURATION ${DATACARD_DIR}/Conf_multi_helix_full.py -W $MAX_PARALLEL -C $SIMULATE_VIRTUALS_EXE -O $DATA_OUT -N $NUMBER_JOBS

$COMMAND $MULTI_MAUS_SCRIPT $DURATION ${DATACARD_DIR}/Conf_multi_straight_complete.py -W $MAX_PARALLEL -C $SIMULATE_STANDARD_EXE -O $DATA_OUT -N $NUMBER_JOBS

$COMMAND $MULTI_MAUS_SCRIPT $DURATION ${DATACARD_DIR}/Conf_multi_helix_complete.py -W $MAX_PARALLEL -C $SIMULATE_STANDARD_EXE -O $DATA_OUT -N $NUMBER_JOBS

echo "Done"
