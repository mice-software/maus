#!/usr/bin/env python

"""
Simple example showing use of ReducePyScalersTable and OutputPyJSON.
"""

import json
import io
import os
import MAUS
import time
import libMausCpp
import ROOT
from Configuration import Configuration
from _InputCppDAQOfflineData import InputCppDAQOfflineData

def run():
    """
    Create a JSON document and create a histogram.
    """
    if not os.environ.get("MAUS_ROOT_DIR"):
        raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
    config = Configuration().getConfigJSON()
    config_json = json.loads(config)
    datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")

    # Set up a run configuration
    datafile = '05466'
    if os.environ['MAUS_UNPACKER_VERSION'] == "StepIV":
        datafile = '06008'
    config_json["daq_data_path"] = datapath
    config_json["daq_data_file"] = datafile

    # Set up data cards.
    data_cards_list = []
    data_cards_list.append("output_file_name='%s'\n" % "scalers")
    data_cards_list.append("output_file_auto_number=%s\n" % True)
    data_cards_list.append("daq_data_path='%s'\n" % datapath)
    data_cards_list.append("daq_data_file='%s'\n" % datafile)
    data_cards = io.StringIO(unicode("".join(data_cards_list)))
    print data_cards

    # Create workers.
    # inputter = InputCppDAQOfflineData(datapath, datafile)
    # inputter.birth(json.dumps(config_json))
    inputter = InputCppDAQOfflineData()

    mappers = MAUS.MapPyGroup()
    mappers.append(MAUS.MapPyDoNothing())  

    reducer = MAUS.ReducePyScalersTable()

    outputter = MAUS.OutputPyFile()

    # Execute the workers.
    MAUS.Go(inputter, mappers, reducer, outputter, data_cards)

if __name__ == "__main__":
    run()
