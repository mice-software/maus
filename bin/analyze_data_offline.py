#!/usr/bin/env python

"""
Reconstruct data from the MICE experiment

Offline analysis to produce reconstructed elements for MICE. TOF is
reconstructed through to space points; Ckov is reconstructed through to Digits.
"""

import MAUS

def run():
    """
    Analyze data from the MICE experiment
    """

    # Set up the input that reads from DAQ
    my_input = MAUS.InputCppDAQOfflineData()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()

    # Trigger
    my_map.append(MAUS.MapCppReconSetup())

    # Detectors
    my_map.append(MAUS.MapCppTOFDigits())
    my_map.append(MAUS.MapCppTOFSlabHits())
    my_map.append(MAUS.MapCppTOFSpacePoints())

    my_map.append(MAUS.MapCppCkovDigits())

    my_map.append(MAUS.MapCppKLDigits())
    my_map.append(MAUS.MapCppKLCellHits())

    my_map.append(MAUS.MapCppTrackerDigits()) # SciFi real data digitization
    my_map.append(MAUS.MapCppTrackerClusterRecon()) # SciFi channel clustering
    my_map.append(MAUS.MapCppTrackerSpacePointRecon()) # SciFi spacepoint recon
    my_map.append(MAUS.MapCppTrackerPatternRecognition()) # SciFi track finding
    my_map.append(MAUS.MapCppTrackerPRSeed()) # Set the Seed from PR
    my_map.append(MAUS.MapCppTrackerTrackFit()) # SciFi track fit

    my_map.append(MAUS.MapCppEMRPlaneHits())
    my_map.append(MAUS.MapCppEMRSpacePoints())
    my_map.append(MAUS.MapCppEMRRecon())

    my_reduce = MAUS.ReducePyDoNothing()

    #  The Go() drives all the components you pass in then put all the output
    #  into a file called 'mausput'
    MAUS.Go(my_input, my_map, my_reduce, MAUS.OutputCppRoot())

if __name__ == '__main__':
    run()

