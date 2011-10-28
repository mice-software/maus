#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""Simulate the MICE experiment, saving ADC/TDC counts as histograms

This will simulate 'number_of_spills' MICE events through the entirity
of MICE using Geant4. At present, TOF and Tracker hits will be digitized.
This output histograms of TDC against ADC counts for each spill and the
total spills processed.
"""

import io   # Generic python library for I/O.
import gzip # For compressed output # pylint: disable=W0611
import os   # For current working directory.

import MAUS

def run():
    """ Run the macro
    """

    # This input generates empty spills, to be filled by the beam maker 
    # later on.
    my_input = MAUS.InputPySpillGenerator()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapPyBeamMaker()) # beam construction
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    my_map.append(MAUS.MapCppTOFDigitization())  #  TOF electronics model
    my_map.append(MAUS.MapCppTrackerDigitization())  # SciFi electronics model

    # Create a reducer. Image type specified in datacards below.
    my_reduce = MAUS.ReducePyDoNothing()

    datacards = io.StringIO()

    # Construct a MAUS output worker - filename and directory
    # comes from datacards.
    my_output = MAUS.OutputPyJSON()

    # Go() drives all the components you pass in. Histograms
    # will be placed in the current directory.
    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

if __name__ == '__main__':
    run()
