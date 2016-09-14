#!/usr/bin/env python

"""
Simulate the MICE experiment

This will simulate MICE spills through the entirety of MICE using Geant4, then
digitize and reconstruct TOF and tracker hits to space points.
"""

import io   #  generic python library for I/O

import MAUS # MAUS libraries

def run():
    """ Run the macro
    """

    # Use the G4BL JSON chunks as an input to the simulation
    my_input = MAUS.InputPyJSON()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()

    # No need for the beam maker, as we use G4BL chunks
    # my_map.append(MAUS.MapPyBeamMaker()) # beam construction

    # Run the GEANT4 simulation
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation

    # Pre detector set up
    my_map.append(MAUS.MapCppMCReconSetup())  #  geant4 simulation

    # TOF
    my_map.append(MAUS.MapCppTOFMCDigitizer())  # TOF MC Digitizer
    my_map.append(MAUS.MapCppTOFSlabHits()) # TOF MC Slab Hits
    my_map.append(MAUS.MapCppTOFSpacePoints()) # TOF Space Points

    # KL
    my_map.append(MAUS.MapCppKLMCDigitizer())  # KL MC Digitizer
    my_map.append(MAUS.MapCppKLCellHits())  # KL CellHit Reco

    # SciFi
    my_map.append(MAUS.MapCppTrackerMCDigitization()) # SciFi electronics model
    my_map.append(MAUS.MapCppTrackerClusterRecon()) # SciFi channel clustering
    my_map.append(MAUS.MapCppTrackerSpacePointRecon()) # SciFi spacepoint recon
    my_map.append(MAUS.MapCppTrackerPatternRecognition()) # SciFi track finding
    my_map.append(MAUS.MapCppTrackerTrackFit()) # SciFi track fit

    # EMR
    my_map.append(MAUS.MapCppEMRMCDigitization())  # EMR MC Digitization
    my_map.append(MAUS.MapCppEMRSpacePoints())  # EMR MC Digitization
    my_map.append(MAUS.MapCppEMRRecon()) # EMR Recon

    # Ckov
    my_map.append(MAUS.MapCppCkovMCDigitizer())

    # Global Digits - post detector digitisation

    # Then construct a MAUS output component - filename comes from datacards
    my_output = MAUS.OutputCppRoot()

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, datacards)

if __name__ == '__main__':
    run()
