#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O
import sys # for command line arguments
import os
import MAUS 

def run(number_of_events, data_path, data_file):
    """Analyze data from the MICE experiment
    
    This will simulate 'number_of_events' MICE events through the entirity
    of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
    """

    # Here we create a pseudo-file with an event in it.  If you were to copy
    # and paste this to a file, then you could also do:
    #
    #   documentFile = open('myFileName.txt', 'r')
    #
    # where the file format has a JSON document per line.  I just toss the file
    # in here for simplicity.
    my_input = MAUS.InputCppRealData(data_path, data_file)
    
    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapPyPrint)
    
    #  The Go() drives all the components you pass in, then check the file
    #  'mausput' for the output
    
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), MAUS.OutputPyJSON())


if __name__ == '__main__':
    if not os.environ.get("MAUS_ROOT_DIR"):
        raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

    # Temporary values for the data path and data file
    data_path = '%s/src/input/InputCppRealData' % os.environ.get("MAUS_ROOT_DIR")
    data_file = '02873.003'
    
    if len(sys.argv) == 1:
        print
        print "WARNING: Usage: %s data_path data_file" % sys.argv[0]
        print
    elif len(sys.argv) == 3:
        data_path = sys.argv[1]
        data_file = sys.argv[2]

    number_of_events = 2

    print "Number of events:", number_of_events
    print "Data path:", data_path
    print "Data file", data_file

    run(number_of_events, data_path, data_file)
