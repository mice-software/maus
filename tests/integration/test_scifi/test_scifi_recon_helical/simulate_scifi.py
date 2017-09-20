#!/usr/bin/env python

"""Simulate the MICE SciFi Tracker

This will simulate 'number_of_spills' MICE spills  through MICE
using Geant4.  Tracker hits will be digitized, reconstructed and
spacepoints and pattern recognition tracks displayed.
"""

import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run():
    """ Run the macro """

    my_input = MAUS.InputPySpillGenerator()

    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapPyBeamMaker()) # beam construction
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    my_map.append(MAUS.MapCppTrackerMCDigitization())  # SciFi electronics model
    my_map.append(MAUS.MapCppTrackerClusterRecon()) # SciFi channel clustering
    my_map.append(MAUS.MapCppTrackerSpacePointRecon()) # SciFi spacepoint recon
    my_map.append(MAUS.MapCppTrackerPatternRecognition()) # SciFi track finding
    my_map.append(MAUS.MapCppTrackerPRSeed()) # Set the Seed from PR
    my_map.append(MAUS.MapCppTrackerTrackFit()) # SciFi track fit

    datacards = io.StringIO(u"")

    # reducer = MAUS.ReduceCppPatternRecognition()
    reducer = MAUS.ReducePyDoNothing()

    # my_output = MAUS.OutputPyJSON()
    my_output = MAUS.OutputCppRoot()

    MAUS.Go(my_input, my_map, reducer, my_output, datacards)

if __name__ == '__main__':
    run()
