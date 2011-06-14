#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O

import MAUS

my_datacards = """
keep_tracks = False
simulation_geometry_filename = "PillBox.dat"
"""

my_input = """{"mc":[{"position":{"x":0.0, "y":-0.0, "z":-5000.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}]}"""


def run(number_of_spills):
    """Simulate the MICE experiment
    
    This will simulate 'number_of_spills' MICE events through the entirity
    of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
    """
    global my_input
    # Here we create a pseudo-file with an event in it.  If you were to copy
    # and paste this to a file, then you could also do:
    #
    #   documentFile = open('myFileName.txt', 'r')
    #
    # where the file format has a JSON document per line.  I just toss the file
    # in here for simplicity.
    document_file = io.StringIO(number_of_spills*unicode(my_input))
    print my_input
    print document_file.getvalue()
    my_input = MAUS.InputPyJSON(document_file)
    
    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapCppSimulation()  #  geant4 simulation
    
    #  You may specify datacards if you wish.  To do so you create a file object
    #  which can either be a StringIO object or a native python file.  If you
    #  want to store your datacards in a file 'datacards.dat' then uncomment:
    # datacards = open('datacards.dat', 'r')
    
    #  The Go() drives all the components you pass in, then check the file
    #  'mausput' for the output
    
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), MAUS.OutputPyJSON(), io.StringIO(unicode(my_datacards)))


if __name__ == '__main__':
    number_of_spills = 1
    run(number_of_spills)
