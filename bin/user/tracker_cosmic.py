#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""Simulate the MICE experiment

This will simulate 'number_of_spills' MICE events through the entirity
of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
"""

import os
import sys
import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run(data_path, run_num):

    my_input = MAUS.InputCppDAQOfflineData(data_path, run_num)

    my_map = MAUS.MapPyGroup()

    my_map.append(MAUS.MapCppTrackerRecon())  # SciFi recon

    datacards = io.StringIO(u"")

    my_output = MAUS.OutputPyJSON()

    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, datacards)

if __name__ == '__main__':
  if not os.environ.get("MAUS_ROOT_DIR"):
    raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

  # Temporary values for the data path and data file
  data_path = os.environ.get("MAUS_ROOT_DIR") + '/src/input/InputCppDAQData'
  data_file = 'gdc1901.001'

  # Check command line arguments for path to data and file name
  if len(sys.argv) == 1:
    print
    print "WARNING: You did not pass in arguments.  I will pick"
    print "WARNING: a run for you..."
    print "WARNING:"
    print "WARNING: Usage: %s data_path data_file" % sys.argv[0]
    print
  elif len(sys.argv) == 3:
    data_path = sys.argv[1]
    data_file = sys.argv[2]

  print "Data path:", data_path
  print "Data file", data_file
  print

  run(data_path, data_file)
