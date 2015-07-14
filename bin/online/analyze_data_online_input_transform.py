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
    """
    Analyze data from the MICE experiment
    This reads in and processes data taken from the MICE
    experiment.
    """
    # Set up data cards.
    data_cards_list = []
    # batch mode = runs ROOT in batch mode so that canvases are not displayed
    # 1 = True, Batch Mode
    # 0 = False, Interactive Mode
    # setting it to false/0 will cause canvases to pop up on screen and 
    # will get refreshed every N spills set by the refresh_rate data
    # card. 
    data_cards_list.append("root_batch_mode='%d'\n" % 1)
    # refresh_rate = once in how many spills should canvases be updated
    data_cards_list.append("refresh_rate='%d'\n" % 1)
    # Add auto-numbering to the image tags. If False then each
    # histogram output for successive spills will have the same tag
    # so there are no spill-specific histograms. This is the
    # recommended use for online reconstruction.
    data_cards_list.append("histogram_auto_number=%s\n" % False)
    # Default image type is eps. For online use, use PNG.
    data_cards_list.append("histogram_image_type=\"png\"\n")

    # Convert data_cards to string.    
    data_cards = io.StringIO(unicode("".join(data_cards_list)))

    # Set up the input that reads from DAQ
    my_input = MAUS.InputCppDAQOnlineData() # pylint: disable = E1101

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    # add ReconSetup map -- analyze_data_offline seems to have it already
    my_map.append(MAUS.MapPyReconSetup())
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())
    my_map.append(MAUS.MapPyCkov())
    my_map.append(MAUS.MapCppKLDigits())
    my_map.append(MAUS.MapCppKLCellHits())
    my_map.append(MAUS.MapCppTrackerDigits())
    my_map.append(MAUS.MapCppTrackerRecon())
    #my_map.append(MAUS.MapCppSingleStationRecon())
    # Histogram reducer.
    reducer = MAUS.ReducePyDoNothing()
    # write out json
    output_worker = MAUS.OutputPyJSON()

    # Run the workflow.
    MAUS.Go(my_input, my_map, reducer, output_worker, data_cards) 

if __name__ == '__main__':
    run()
