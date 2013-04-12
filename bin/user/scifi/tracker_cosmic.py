#!/usr/bin/env python

"""Reconstruct Tracker Cosmic Data"""

import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run():
    """ Run the macro """

    my_input = MAUS.InputCppDAQOfflineData()
    # my_input = MAUS.InputPyJSON()

    my_map = MAUS.MapPyGroup()

    my_map.append(MAUS.MapCppTrackerDigits())

    my_map.append(MAUS.MapCppTrackerRecon())  # SciFi recon

    datacards = io.StringIO(u"")

    # my_output = MAUS.OutputPyJSON()
    my_output = MAUS.OutputCppRoot()

    # my_reduce = MAUS.ReducePyDoNothing()
    my_reduce = MAUS.ReduceCppPatternRecognition()

    MAUS.Go(my_input, my_map, my_reduce, my_output, datacards)

if __name__ == '__main__':
    run()
