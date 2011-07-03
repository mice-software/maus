
import json
import types
import os
import io


class MapPyScalersAnalysis:
    def birth(self, configJSON):
        return True

    def process(self, str):
        spill = json.loads(str)

        if 'daq_data' not in spill:
            return str

        if spill['daq_data'] is None:
            return str

        for subdata in spill['daq_data']:
            if subdata is None:
                continue

            if 'scaler' in subdata.keys():
                 print subdata['scaler'].keys()
                 scalers = spill['scaler']

        return str

    def death(self):
        return True






