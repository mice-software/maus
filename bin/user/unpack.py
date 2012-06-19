#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O
import sys # for command line arguments
import os
import MAUS

def run(data_path, run_num):
    """Analyze data from the MICE experiment

    This will read in and process data taken from the MICE experiment. It will
    eventually include things like cabling information, calibrations, and fits.
    """

    # Here you specify the path to the data and also the file you want to
    # analyze.

    my_input = MAUS.InputCppDAQOfflineData(data_path, data_file)

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppTrackerDigits())
    my_map.append(MAUS.MapCppTrackerRecon())

    #reducer = MAUS.ReduceCppTracker()
    reducer = MAUS.ReducePyDoNothing()
    # reducer = MAUS.ReduceCppTrackerErrorLog()

    output_file = open("unpacked_1901", 'w')  #  Uncompressed
    my_output = MAUS.OutputPyJSON(output_file)

    # The Go() drives all the components you pass in then put all the output
    # into a file called 'mausput'
    MAUS.Go(my_input, my_map, reducer, my_output)


if __name__ == '__main__':
    if not os.environ.get("MAUS_ROOT_DIR"):
        raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

    # Temporary values for the data path and data file

    data_path = '../../../../cosmic_raw/'  #'%s/src/input/InputCppDAQData' % os.environ.get("MAUS_ROOT_DIR")
    data_file = 'gdc1901.000 gdc1901.001'
    
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
