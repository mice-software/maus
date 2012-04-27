#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O
import sys # for command line arguments
import os
import MAUS

def run():
    """Analyze data from the MICE experiment

    This will read in and process data taken from the MICE experiment. It will
    eventually include things like cabling information, calibrations, and fits.
    """

    # Here you specify the path to the data and also the file you want to
    # analyze.

    my_input = MAUS.InputCppDAQOnlineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapPyScalersDump())
    #my_map.append(MAUS.MapCppTOFDigits())
    #my_map.append(MAUS.MapCppTOFSlabHits())
    #my_map.append(MAUS.MapCppTOFSpacePoints())
    #my_map.append(MAUS.MapPyTOFPlot())


    #  The Go() drives all the components you pass in then put all the output
    #  into a file called 'mausput'
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), MAUS.OutputPyJSON())


if __name__ == '__main__':
    if not os.environ.get("MAUS_ROOT_DIR"):
        raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

    run()
