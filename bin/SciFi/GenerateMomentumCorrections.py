#!/usr/bin/env python

"""
Generate an MC data file and calculate the required Pattern Recognition
momentum corrections required for the track reconstruction.

This will simulate MICE spills through the entirety of MICE using Geant4, then
digitize and reconstruct tracker hits to space points. Finally a 
reducer is used to analysis the MC truth and reconstructed tracks in order to 
calculate the required corrections.
"""

import io   #  generic python library for I/O
import os

import MAUS # MAUS libraries

config_file = os.path.join(os.getenv("MAUS_ROOT_DIR"),
                                   "bin/SciFi/Conf_PR_Momentum_Corrections.py")

def run():
    """ Run the macro
    """

    # This input generates empty spills, to be filled by the beam maker later on
    my_input = MAUS.InputPySpillGenerator()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()

    # GEANT4
    my_map.append(MAUS.MapPyBeamMaker()) # beam construction
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation

    # Pre detector set up
    # my_map.append(MAUS.MapPyMCReconSetup())  #  geant4 simulation
    my_map.append(MAUS.MapCppMCReconSetup())  #  geant4 simulation


    # SciFi
    my_map.append(MAUS.MapCppTrackerMCDigitization()) # SciFi electronics model
    my_map.append(MAUS.MapCppTrackerClusterRecon()) # SciFi channel clustering
    my_map.append(MAUS.MapCppTrackerSpacePointRecon()) # SciFi spacepoint recon
    my_map.append(MAUS.MapCppTrackerPatternRecognition()) # SciFi track finding
#    my_map.append(MAUS.MapCppTrackerTrackFit()) # SciFi track fit

    # Momentum Corrections Reducer
    my_reduce = MAUS.ReduceCppSciFiMomentumCorrections()

    # Then construct a MAUS output component - filename comes from datacards
    my_output = MAUS.OutputCppRoot()

    # can specify datacards here or by using appropriate command line calls
    datacards = open(config_file, 'r')

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

if __name__ == '__main__':
    run()
