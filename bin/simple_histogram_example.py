#!/usr/bin/env python
"""
Simple example showing use of ReducePyMatplotlibHistogram and
OutputPyImage.
"""

import json
import io
import MAUS
import sys

def run(image_type = None, file_prefix = None, directory_path = None):
    """
    Create a JSON document and create a histogram.
    @param image_type Image type.
    @param file_prefix Prefix for file names.
    @param directory_path Directory for files.
    """

    input_docs = []
    for i in range(0, 4):
        json_doc = {}
        json_doc["digits"] = []
        for j in range(1, 10):
#            digit = {"ac_counts":i, "tc_counts":j}
#            digit = {"adc_counts":i,
#                     "channel_id":{"fiber_number":106,
#                     "plane_number":2, "station_number":1,
#                     "tracker_number":0, "type":"Tracker"}}
#            digit = {"channel_id":{"fiber_number":106,
#                     "plane_number":2, "station_number":1,
#                     "tracker_number":0, "type":"Tracker"},
#                     "tdc_counts":j}
            digit = {"adc_counts":i,
                     "channel_id":{"fiber_number":106,
                     "plane_number":2, "station_number":1,
                     "tracker_number":0, "type":"Tracker"},
                     "tdc_counts":j}
            json_doc["digits"].append(digit)
        input_docs.append(json.dumps(json_doc))
        input_docs.append("\n")
    input_file = io.StringIO(unicode("".join(input_docs)))

    data_cards = []
    if (image_type != None):
        data_cards.append("histogram_image_type='%s'\n" % image_type)
    if (file_prefix != None):
        data_cards.append("histogram_file_prefix='%s'\n" % file_prefix)
    if (directory_path != None):
        data_cards.append("histogram_directory='%s'\n" % directory_path)
    data_cards_file = io.StringIO(unicode("".join(data_cards)))

    input_worker = MAUS.InputPyJSON(input_file)
    map_worker = MAUS.MapPyGroup()
    map_worker.append(MAUS.MapPyDoNothing())  
    output_worker = MAUS.OutputPyImage()

    MAUS.Go(input_worker, 
            map_worker, 
            MAUS.ReducePyMatplotlibHistogram(), 
            output_worker, 
            data_cards_file)

if __name__ == "__main__":
    image_type = None
    file_prefix = None
    directory_path = None
    if len(sys.argv) == 4:
        image_type = sys.argv[1]
        file_prefix = sys.argv[2]
        directory_path = sys.argv[3]
    run(image_type, file_prefix, directory_path)
