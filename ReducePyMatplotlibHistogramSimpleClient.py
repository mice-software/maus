#!/usr/bin/env python

from Configuration import Configuration
import io
import json
import MAUS

# Build JSON docs.
jsonStrs = []
for i in range(1, 3):
    jsonDoc = {}
    jsonDoc["digits"] = []
    for j in range(1, 10):
#        digit = {"ac_counts":i * j,"tc_counts":i * j * 2}
        digit = {"adc_counts":i * j,"channel_id":{"fiber_number":106,"plane_number":2,"station_number":1,"tracker_number":0,"type":"Tracker"},"tdc_counts":i * j * 2}
        jsonDoc["digits"].append(digit)
    jsonStrs.append(json.dumps(jsonDoc))

reducer = MAUS.ReducePyMatplotlibHistogram()
configJson = Configuration()
reducer.birth(configJson.getConfigJSON())

for jsonStr in jsonStrs:
    print reducer.process(jsonStr, jsonStr)

reducer.death()
