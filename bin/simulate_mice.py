#!/usr/bin/env python
import io  #  generic python library for I/O

from InputPyJSON import *  #  read in raw JSON document to start
from MapPyGroup import *  #  chain map steps together
from MapCppSimulation import *  #  used for geant4 simulation
from MapCppTOFDigitization import *  #  TOF digitize hits
from MapCppTrackerDigitization import *  #  tracker digitize hits
from ReducePyDoNothing import *  #  do nothing
from OutputPyJSON import *  #  write output to JSON

# All components are tied together with this Go()
# class.  This is the 'core' of MAUS.
from Go import * 


# Here we create a pseudo-file with an event in it.  If you were to copy
# and paste this to a file, then you could also do:
# 
#   documentFile = open('myFileName.txt', 'r')
#
# where the file format has a JSON document per line.  I just toss the file
# in here for simplicity.
number_of_events = 1
documentFile = io.StringIO(number_of_events*u"""{"mc": [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210, "random_seed" : 10, "unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n""")

myInput = InputPyJSON(documentFile)

# Create an empty array of mappers, then populate it
# with the functionality you want to use.
myMap = MapPyGroup()
myMap.append(MapCppSimulation())  #  geant4 simulation 
myMap.append(MapCppTOFDigitization())  #  TOF detector digitization
myMap.append(MapCppTrackerDigitization())  #  tracker detector digitization

#  The Go() drives all the components you pass in, then check the file
#  'mausput' for the output
Go(myInput, myMap, ReducePyDoNothing(), OutputPyJSON(), io.StringIO(u"keep_tracks = True"))
