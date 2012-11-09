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

    # TOF
    # my_map.append(MAUS.MapCppTOFMCDigitizer())  # TOF MC Digitizer
    # my_map.append(MAUS.MapCppTOFSlabHits()) # TOF MC Slab Hits
    # my_map.append(MAUS.MapCppTOFSpacePoints()) # TOF Space Points

    # SciFi
    # my_map.append(MAUS.MapCppTrackerMCDigitization()) # SciFi electronics model
#    my_map.append(MAUS.MapCppTrackerRecon()) # SciFi Recon

    # Global Digits - post detector digitisation

    my_map.append(MAUS.MapPySmearMC())  #  perform a Gaussian smear on MC hits

    # Then construct a MAUS output component - filename comes from datacards
    my_output = MAUS.OutputPyJSON()

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, datacards)

if __name__ == '__main__':
    run()
