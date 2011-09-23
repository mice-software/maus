#!/usr/bin/env python
"""Simple example showing use of ReducePyMatplotlibHistogram."""

from Configuration import Configuration
import json
import io
import MAUS

def run():
    """Create a JSON document and create a histogram."""

    json_strs = []
    for i in range(1, 4):
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
        json_strs.append(json.dumps(json_doc))

    reducer = MAUS.ReducePyMatplotlibHistogram()
    config = Configuration()
    config_json = json.loads(config.getConfigJSON())
    config_json["histogram_image_type"] = "eps"
    config_json["histogram_file_prefix"] = "plot"
    config_json["histogram_directory"] = "test_plots"

    reducer.birth(json.dumps(config_json))

    output = ""
    for json_str in json_strs:
        output = reducer.process(output, json_str)

    reducer.death()

    output_file = io.StringIO(unicode(output))
    next_value = output_file.readline()
    while next_value != "":
        next_value = next_value.rstrip()
        if next_value != "":
            json_doc = json.loads(next_value)
            if "histograms" in json_doc:
                for entry in json_doc["histograms"]:
                    print "Saving %s to %s" \
                        % (entry["content"], entry["file_path"])
                    data_file = open(entry["file_path"], "w")
                    data_file.write(entry["data"])
                    data_file.close()
        next_value = output_file.readline()

if __name__ == "__main__":
    run()
