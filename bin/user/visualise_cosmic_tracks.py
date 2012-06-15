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

def run():

    my_input = MAUS.InputPyJSON()

    my_map = MAUS.MapPyDoNothing()

    my_reduce = MAUS.ReduceCppPatternRecognition()

    my_output = MAUS.OutputPyDoNothing()

    datacards = io.StringIO(u"")

    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

if __name__ == '__main__':
  if not os.environ.get("MAUS_ROOT_DIR"):
    raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

  run()