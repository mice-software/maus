#!/usr/bin/env python
"""simulate_mice.py"""
#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O

import MAUS

def run(number_of_spills):#pylint: disable =W0621
    """Simulate the MICE experiment
    
    This will simulate 'number_of_spills' MICE events through the entirity
    of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
    """

    # Here we create a pseudo-file with an event in it.  If you were to copy
    # and paste this to a file, then you could also do:
    #
    #   documentFile = open('myFileName.txt', 'r')
    #
    # where the file format has a JSON document per line.  I just toss the file
    # in here for simplicity.
    document_file = io.StringIO(number_of_spills*u"""{"mc": [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210, "random_seed" : 10, "unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n""")#pylint: disable =C0301
    
    my_input = MAUS.InputPyJSON(document_file)
    
    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppSimulation())  #  geant4 simulation
    #TOF detector digitization
    my_map.append(MAUS.MapCppTOFDigitization())#pylint: disable=E1101
    #  tracker detector digitization
    my_map.append(MAUS.MapCppTrackerMCDigitization())
    
    datacards = io.StringIO(u"keep_tracks = False\n"\
                            "simulation_geometry_filename = \"Stage5.dat\"")
    #  You may specify datacards if you wish.  To do so you create a file object
    #  which can either be a StringIO object or a native python file.  If you
    #  want to store your datacards in a file 'datacards.dat' then uncomment:
    # datacards = open('datacards.dat', 'r')
    
    #  The Go() drives all the components you pass in, then check the file
    #  'mausput' for the output
    
    MAUS.Go(my_input,
            my_map,
            MAUS.ReducePyDoNothing(),
            MAUS.OutputPyJSON(),
            datacards)


if __name__ == '__main__':
    number_of_spills = 10#pylint: disable =C0103
    run(number_of_spills)
