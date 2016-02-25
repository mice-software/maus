#!/usr/bin/env python

"""
Process unpacked real data incl. local reconstruction for all detectors
(see MAUS User Guide for unpacking instructions). After unpacking the data,
use this script with process_datecard.py and adjust parameters as needed.
Make sure to use a no-field geometry (only no-field data can be matched
currently, due to an issue with the Geant4 geometry handling)
Refer to ConfigurationDefaults for documentation of the track_matching_*
parameters in the datacard.
track_matching_energy_loss needs to be False, as setting it to true will cause
segfaults due to the aforementioned issue.
"""

import io   #  generic python library for I/O

import MAUS # MAUS libraries

def run():
    """ Run the macro
    """

    # This input generates empty spills, to be filled by the beam maker later on
    my_input = MAUS.InputCppRoot()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()

    # Global
    my_map.append(MAUS.MapCppGlobalReconImport())
    my_map.append(MAUS.MapCppGlobalTrackMatching())

    my_reduce = MAUS.ReducePyDoNothing()

    # Then construct a MAUS output component - filename comes from datacards
    my_output = MAUS.OutputCppRoot()

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

#MAUS.ReducePyDoNothing()
if __name__ == '__main__':
    run()
