#!/usr/bin/env python

"""
Reconstruct data from the MICE experiment

Offline analysis to produce reconstructed elements for MICE. TOF is
reconstructed through to space points; Ckov is reconstructed through to Digits.
"""

import MAUS

def run():
    """
    Analyze data from the MICE experiment
    """

    # Set up the input that reads from DAQ
    my_input = MAUS.InputCppDAQOfflineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()

    # Trigger
    my_map.append(MAUS.MapCppReconSetup())

    # Detectors
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())

    my_map.append(MAUS.MapCppTrackerDigits())
    my_map.append(MAUS.MapCppTrackerRecon())

    # Data selection
    my_map.append(MAUS.MapCppDataSelection())

    # No reducer
    my_reduce = MAUS.ReducePyDoNothing()

    #  The Go() drives all the components you pass in then put all the output
    #  into a file called 'mausput'
    MAUS.Go(my_input, my_map, my_reduce, MAUS.OutputCppRoot())

if __name__ == '__main__':
    run()

