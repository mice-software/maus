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

    my_output = MAUS.OutputPyDoNothing()

    # datacard specifies the hypothesis for which PDFs are to be made
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in
    MAUS.Go(my_input, MAUS.MapPyDoNothing(), MAUS.ReduceCppGlobalPID(),
            my_output, datacards)

if __name__ == '__main__':
    run()
