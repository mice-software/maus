#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O

import MAUS

from InputPyJSON import InputPyJSON  #  read in raw JSON document to start
from MapPyGroup import MapPyGroup  #  chain map steps together
from MapCppSimulation import MapCppSimulation  #  used for geant4 simulation
from MapCppTOFDigitization import MapCppTOFDigitization 
from MapCppTrackerDigitization import MapCppTrackerDigitization
from MapPyCreateSpill import MapPyCreateSpill
from MapPyBeamMaker import MapPyBeamMaker
from MapPyPrint import MapPyPrint
from ReducePyDoNothing import ReducePyDoNothing  #  do nothing
from OutputPyJSON import OutputPyJSON  #  write output to JSON

# All components are tied together with this Go()
# class.  This is the 'core' of MAUS.
from Go import Go


def run(number_of_spills):
    """Simulate the MICE experiment
    
    This will simulate 'number_of_events' MICE events through the entirity
    of MICE using Geant4.  At present, TOF and Tracker hits will be digitized.
    """

    # Here we create a pseudo-file with an event in it.  If you were to copy
    # and paste this to a file, then you could also do:
    #
    #   documentFile = open('myFileName.txt', 'r')
    #
    # where the file format has a JSON document per line.  I just toss the file
    # in here for simplicity.
 #    document_file = io.StringIO(number_of_events*u"""{"mc": [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210, "random_seed" : 10, "unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n""")
    
    my_input = MAUS.InputPyEmptyDocument(number_of_spills)
    
    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MapPyGroup()
  #  my_map.append(MapPyPrint())
    my_map.append(MapPyCreateSpill())
   # my_map.append(MapPyPrint())
    my_map.append(MapPyBeamMaker())
   # my_map.append(MapCppSimulation())  #  geant4 simulation
    
   
    my_map.append(MapPyPrint())
    #my_map.append(MapCppTOFDigitization())  #  TOF detector digitization
    #my_map.append(MapCppTrackerDigitization())  #  tracker detector digitization
    
    datacards = io.StringIO(u"keep_tracks = False")
    #  You may specify datacards if you wish.  To do so you create a file object
    #  which can either be a StringIO object or a native python file.  If you
    #  want to store your datacards in a file 'datacards.dat' then uncomment:
    # datacards = open('datacards.dat', 'r')
    
    #  The Go() drives all the components you pass in, then check the file
    #  'mausput' for the output
    
    Go(my_input, my_map, ReducePyDoNothing(), OutputPyJSON(), datacards)


if __name__ == '__main__':
    number_of_spills = 1
    run(number_of_spills)
    
