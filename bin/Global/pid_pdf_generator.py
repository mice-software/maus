#!/usr/bin/env python

"""
Produce the MC probability density functions for MC data that has first been
processed by MapCppGlobalReconImport and MapCppGlobalTrackMatching. Several
datacards must be set, an example configuration file is included in
bin/Global/pdf_example_datacard.py
"""

import io   #  generic python library for I/O

import MAUS # MAUS libraries

def run():
    """ Run the macro
    """

    # Take as input a maus root file.
    my_input = MAUS.InputCppRootData()

    my_output = MAUS.OutputPyDoNothing()

    # datacard specifies the hypothesis for which PDFs are to be made
    datacards = io.StringIO(u"")

    # The Go() drives all the components you pass in
    MAUS.Go(my_input, MAUS.MapPyDoNothing(), MAUS.ReduceCppGlobalPID(),
            my_output, datacards)

if __name__ == '__main__':
    run()
