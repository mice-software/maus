#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io   #  generic python library for I/O
import gzip #  For compressed output

import MAUS

def run(number_of_spills):
    """Simulate the MICE experiment
    
    This will simulate 'number_of_spills' MICE events through the entirity
    of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
    """

    # Here we create a pseudo-file with an event in it.  If you were to copy
    # and paste this to a file, then you could also do:
    #
    #   input_file = open('myFileName.txt', 'r')
    #
    # where the file format has a JSON document per line.  I just toss the file
    # in here for simplicity.
    input_file = io.StringIO(number_of_spills*u"""{"mc": [{"primary":{"position": { "x": 0.0, "y": -0.0, "z": -5000.0 },"particle_id" : 13,"energy" : 210.0, "random_seed" : 10, "momentum" : { "x":0.0, "y":0.0, "z":1.0 }, "time" : 0.0}}]}\n""")
    
    my_input = MAUS.InputPyJSON(input_file)
    
    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    my_map.append(MAUS.MapCppTOFDigitization())  #  TOF detector digitization
    my_map.append(MAUS.MapCppTrackerDigitization())  #  tracker detector digitization
    
    datacards = io.StringIO(u"keep_tracks = False")
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
    #  'mausput' for the output
    
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, datacards)


if __name__ == '__main__':
    number_of_spills = 10
    run(number_of_spills)
