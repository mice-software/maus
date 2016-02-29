#!/usr/bin/env python

"""
Run PID on an input root maus output file (data must have passed through
MapCppGlobalReconImport and MapCppGlobalTrackMatching first). Can also 
accept and produce JSON input/output. Several datacards must be set, default
values are in ConfigurationDefaults.py. An example configuration file for PID
is included in bin/Global/pid_example_datacard.py
"""

import io   #  generic python library for I/O

import MAUS # MAUS libraries

def run():
    """ Run the macro
    """

    # Take as input spills from pre-existing json document, as defined in
    # datacard
    my_input = MAUS.InputCppRoot()

    my_output = MAUS.OutputCppRoot()
    
    my_map = MAUS.MapPyGroup()
    
    my_map.append(MAUS.MapCppGlobalPID())

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(),
            my_output, datacards)

if __name__ == '__main__':
    run()
