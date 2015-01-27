#!/usr/bin/env python

"""
Produce the MC probability density functions for the particle hypothesis
given in the specified datacard.
"""

import io   #  generic python library for I/O

import MAUS # MAUS libraries

def run():
    """ Run the macro
    """

    # Take as input spills from pre-existing json document, as defined in
    # datacard
    my_input = MAUS.InputPyJSON()

    my_output = MAUS.OutputPyJSON()
    
    my_map = MAUS.MapPyGroup()
    
    my_map.append(MAUS.MapCppGlobalReconImport())

    # can specify datacards here or by using appropriate command line calls
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in, then check the file
    # (default simulation.out) for output
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(),
            my_output, datacards)

if __name__ == '__main__':
    run()
