#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Simple example showing use of ReducePyScalersTable and OutputPyJSON.
"""

import json
import io
import MAUS
import time

def run():
    """
    Create a JSON document and create a histogram.
    """
    # Create sample JSON documents with spills containing data 
    # expected by ReducePyScalersTable.
    input_docs = []
    for i in range(0, 4):
        json_doc = {}
        json_doc["daq_data"] = {}
        json_doc["daq_data"]["V830"] = {}
        scalers = json_doc["daq_data"]["V830"]
        scalers["phys_event_number"] = "Sample event %d" % i
        scalers["time_stamp"] = time.time()
        hits = {}
        hits["ch0"] = i
        hits["ch1"] = i
        hits["ch2"] = i
        hits["ch3"] = i
        hits["ch4"] = i
        hits["ch12"] = i
        scalers["channels"] = hits
        input_docs.append(json.dumps(json_doc))
        print json_doc
        input_docs.append("\n")
    input_file = io.StringIO(unicode("".join(input_docs)))

    # Set up data cards.
    data_cards_list = []
    data_cards_list.append("output_file_prefix='%s'\n" % "scalers")
    data_cards_list.append("output_file_auto_number=%s\n" % True)
    data_cards = io.StringIO(unicode("".join(data_cards_list)))

    # Create workers.
    input_worker = MAUS.InputPyJSON(input_file)

    mappers = MAUS.MapPyGroup()
    mappers.append(MAUS.MapPyDoNothing())  

    reducer = MAUS.ReducePyScalersTable()

    output_worker = MAUS.OutputPyFile()

    # Execute the workers.
    MAUS.Go(input_worker, mappers, reducer, output_worker, data_cards)

if __name__ == "__main__":
    run()
