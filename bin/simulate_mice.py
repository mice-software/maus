#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

"""Simulate the MICE experiment

This will simulate 'number_of_spills' MICE events through the entirity
of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
"""

import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run():
    """ Run the macro
    """

    # This input generates empty spills, to be filled by the beam maker later on
    my_input = MAUS.InputPySpillGenerator()

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapPyBeamMaker()) # beam construction
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    my_map.append(MAUS.MapCppTOFDigitization())  #  TOF electronics model
    my_map.append(MAUS.MapCppTrackerDigitization())  # SciFi electronics model

    datacards = io.StringIO(u"")
    #  You may specify datacards if you wish.  To do so you create a file object
    #  which can either be a StringIO object or a native python file.  If you
    #  want to store your datacards in a file 'datacards.dat' then uncomment:
    # datacards = open('datacards.dat', 'r')


    #  Choose from either a compressed or uncompressed output file
    #
    output_file = open("simulation.out", 'w')  #  Uncompressed
    #output_file = gzip.GzipFile("mausput.gz", 'wb')  #  Compressed

    #
    # Then construct a MAUS output component
    my_output = MAUS.OutputPyJSON(output_file)

    #  The Go() drives all the components you pass in, then check the file
    #  'simulation.out' for the output

    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, datacards)

if __name__ == '__main__':
    run()
