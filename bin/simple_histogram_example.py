#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""
Simple example showing use of ReducePyMatplotlibHistogram and
OutputPyImage. 
"""

import json
import io
import MAUS
import os

def run():
    """
    Create a JSON document and create a histogram.
    """

    # Create sample JSON documents with data that can be histogrammed.

    input_docs = []
    for i in range(0, 4):
        json_doc = {}
        json_doc["digits"] = []
        for j in range(1, 10):
            digit = {"adc_counts":i,
                     "channel_id":{"fiber_number":106,
                     "plane_number":2, "station_number":1,
                     "tracker_number":0, "type":"Tracker"},
                     "tdc_counts":j}
            json_doc["digits"].append(digit)
        input_docs.append(json.dumps(json_doc))
        input_docs.append("\n")
    input_file = io.StringIO(unicode("".join(input_docs)))

    # Set up data cards.
    data_cards_list = []
    # image type must be one of those supported by matplotlib
    # (currently "svg", "ps", "emf", "rgba", "raw", "svgz", "pdf",
    # "eps", "png"). Default: "eps".
    data_cards_list.append("histogram_image_type='%s'\n" % "eps")
    # Prefix for file names. Default: auto-generated UUID.
    data_cards_list.append("image_file_prefix='%s'\n" % "sample-image")
    # Directory for images. Default: current directory.
    data_cards_list.append("image_directory='%s'\n" % os.getcwd())
    data_cards = io.StringIO(unicode("".join(data_cards_list)))

    # Create workers.
    input_worker = MAUS.InputPyJSON(input_file)

    mappers = MAUS.MapPyGroup()
    mappers.append(MAUS.MapPyDoNothing())  

    reducer = MAUS.ReducePyMatplotlibHistogram()

    output_worker = MAUS.OutputPyImage()

    # Execute thr workers.
    MAUS.Go(input_worker, mappers, reducer, output_worker, data_cards)

if __name__ == "__main__":
    run()
