#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Offline analysis to produce reconstructed events from the MICE Experiment
"""

import MAUS

def run():
    """Analyze data from the MICE experiment

    This will read in and process data taken from the MICE experiment. It will
    eventually include things like cabling information, calibrations, and fits.
    """

    # Set up the input that reads from DAQ
    #my_input = MAUS.InputCppDAQData()
    my_input = MAUS.InputCppDAQOfflineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())
    my_map.append(MAUS.MapCppCkovDigits())
    reducer = MAUS.ReducePyCkov()
    #  The Go() drives all the components you pass in then put all the output
    #  into a file called 'mausput'
    MAUS.Go(my_input, my_map, reducer, MAUS.OutputPyImage())

if __name__ == '__main__':
    run()

