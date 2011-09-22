#!/usr/bin/env python
"""Simple example showing use of ReducePyMatplotlibHistogram. """

from Configuration import Configuration
import json
import MAUS

def run():
    """Create a JSON document and create a histogram."""
    # Build JSON docs.
    json_strs = []
    for i in range(1, 4):
        json_doc = {}
        json_doc["digits"] = []
        for j in range(1, 10):
#            digit = {"ac_counts":i,"tc_counts":j}
            digit = {"adc_counts":i,
                     "channel_id":{"fiber_number":106,
                     "plane_number":2, "station_number":1,
                     "tracker_number":0, "type":"Tracker"},
                     "tdc_counts":j}
            json_doc["digits"].append(digit)
        json_strs.append(json.dumps(json_doc))

    reducer = MAUS.ReducePyMatplotlibHistogram()
    config_json = Configuration()
    reducer.birth(config_json.getConfigJSON())
    for json_str in json_strs:
        print reducer.process(json_str, json_str)
    reducer.death()

if __name__ == '__main__':
    run()
