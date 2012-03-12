#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Online analysis to produce reconstructed events from the MICE Experiment.
"""

import MAUS
import io

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
    # Add auto-numbering to the image tags. If False then each
    # histogram output by ReducePyMatplotlibHistogram will have
    # tags "tdcadc" and so the end result will be just one histogram 
    # file. If True then there will be N files, one for each spill.
    data_cards_list.append("histogram_auto_number=%s\n" % False)

    # Directory for images. Default: $MAUS_WEB_MEDIA_RAW if set
    # else current directory.
#    data_cards_list.append("image_directory='%s'\n" % os.getcwd())

    # Convert data_cards to string.
    
    data_cards = io.StringIO(unicode("".join(data_cards_list)))

    # Set up the input that reads from DAQ
#    my_input = MAUS.InputCppDAQData()
#    my_input = MAUS.InputCppDAQOnlineData()
    my_input = MAUS.InputCppDAQOfflineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())
    # Histogram reducer.
    reducer = MAUS.ReducePyTOFPlot()
    # Save images as EPS and meta-data as JSON.
    output_worker = MAUS.OutputPyImage()

    # Run the workflow.
    MAUS.Go(my_input, my_map, reducer, output_worker, data_cards) 

if __name__ == '__main__':
    run()
