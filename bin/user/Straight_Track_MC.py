#!/usr/bin/env python

"""
Simulate the MICE experiment

This will simulate MICE spills through the entirety of MICE using Geant4, then
digitize and reconstruct TOF and tracker hits to space points.
"""

import io   #  generic python library for I/O
import os
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
    my_map.append(MAUS.MapPyMCReconSetup())  #  geant4 simulation

    # TOF
    #my_map.append(MAUS.MapCppTOFMCDigitizer())  # TOF MC Digitizer
    #my_map.append(MAUS.MapCppTOFSlabHits()) # TOF MC Slab Hits
    #my_map.append(MAUS.MapCppTOFSpacePoints()) # TOF Space Points

    # SciFi
    my_map.append(MAUS.MapCppTrackerMCDigitization()) # SciFi electronics model
    my_map.append(MAUS.MapCppTrackerRecon()) # SciFi Recon

    # Global Digits - post detector digitisation

    # my_reduce = MAUS.ReduceCppMCTracker()
    # my_reduce = MAUS.ReduceCppPatternRecognition()
    # my_reduce = MAUS.ReducePyHistogramTDCADCCounts()
    my_reduce = MAUS.ReducePyDoNothing()

    # Then construct a MAUS output component - filename comes from datacards
    my_output = MAUS.OutputPyJSON()
    # my_output = MAUS.OutputCppRoot()
    # my_output = MAUS.OutputPyImage()

    # Can specify datacards here or by using appropriate command line calls.
    # datacards_list = []
    # image type must be one of those supported by matplotlib
    # (currently "svg", "ps", "emf", "rgba", "raw", "svgz", "pdf",
    # "eps", "png"). Default: "eps".
    # datacards_list.append("histogram_image_type='%s'\n" % "eps")
    # Add auto-numbering to the image tags. If False then each 
    # histogram output by ReducePyMatplotlibHistogram will have
    # tags "tdcadc" and so the end result will be just one histogram 
    # file. If True then there will be N files, one for each spill.
    # datacards_list.append("histogram_auto_number=%s\n" % False)
    # Prefix for file names. Default: auto-generated UUID.
    # datacards_list.append("image_file_prefix='%s'\n" % "histogram")
    # Directory for images. Default: current directory.
    # datacards_list.append("image_directory='%s'\n" % os.getcwd())
    # datacards = io.StringIO(unicode("".join(datacards_list)))

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

if __name__ == '__main__':
    run()
