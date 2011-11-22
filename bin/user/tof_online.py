#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Offline analysis to produce reconstructed events from the MICE Experiment
"""

import MAUS
import io
import json
import os

def run():
    """Analyze data from the MICE experiment

    This will read in and process data taken from the MICE experiment. It will
    eventually include things like cabling information, calibrations, and fits.
    """


    # Set up data cards.
    data_cards_list = []
    # batch mode = runs ROOT in batch mode so that canvases are not displayed
    # 1 = True, Batch Mode
    # 0 = False, Interactive Mode
    # setting it to false will cause canvases to pop up on screen and 
    # will get refreshed every N spills set by the refresh_rate data card
    data_cards_list.append("root_batch_mode='%d'\n" % 1)
    # refresh_rate = once in how many spills should canvases be updated
    data_cards_list.append("refresh_rate='%d'\n" % 1)
    data_cards = io.StringIO(unicode("".join(data_cards_list)))

    # Set up the input that reads from DAQ
    data_dir = '%s/src/input/InputCppDAQData' % os.environ.get("MAUS_ROOT_DIR")
    data_file = '02873.003'
    # In Yordan's branch, needed to specify args to InputCppDAQData
    # my_input = MAUS.InputCppDAQData(data_dir,data_file)
    my_input = MAUS.InputCppDAQData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())

    reducer = MAUS.ReducePyTOFPlots()

    output_worker = MAUS.OutputPyJSON()

    #  The Go() drives all the components you pass in then put all the output
    #  into a file called 'mausput'
    MAUS.Go(my_input, my_map, reducer,output_worker,data_cards) 

if __name__ == '__main__':
    run()

