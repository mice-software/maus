#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Online analysis to produce reconstructed events from the MICE Experiment
and create scaler summary outputs.
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
    data_cards_list.append("output_file_name='scalers'\n")
    # Convert data_cards to string.    
    data_cards = io.StringIO(unicode("".join(data_cards_list)))
    # Set up the input that reads from DAQ
    my_input = MAUS.InputCppDAQOfflineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())
    # Histogram reducer.
    reducer = MAUS.ReducePyScalersTable()
    # Save images as EPS and meta-data as JSON.
    output_worker = MAUS.OutputPyFile()

    # Run the workflow.
    MAUS.Go(my_input, my_map, reducer, output_worker, data_cards) 

if __name__ == '__main__':
    run()
