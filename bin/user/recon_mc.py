#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""Simulate the MICE experiment

This will simulate 'number_of_spills' MICE events through the entirity
of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
"""

import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run():
    """ Run the macro
    """

    # This input generates empty spills, to be filled by the beam maker later on
    input_file = open('simulation_stage4', 'r')
    #
    my_input = MAUS.InputPyJSON(input_file)
    #my_input = MAUS.InputPySpillGenerator()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    #my_map.append(MAUS.MapPyBeamMaker()) # beam construction
    #my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    my_map.append(MAUS.MapCppTrackerMCDigitization())  # SciFi electronics model
    my_map.append(MAUS.MapCppTrackerRecon())  # SciFi recon
    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    #reducer = MAUS.ReduceCppTracker()
    reducer = MAUS.ReducePyDoNothing()

    output_file = open("recon_mc", 'w')  #  Uncompressed
    # Then construct a MAUS output component
    my_output = MAUS.OutputPyJSON(output_file)

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output

    MAUS.Go(my_input, my_map, reducer, my_output, datacards)

if __name__ == '__main__':
    run()
