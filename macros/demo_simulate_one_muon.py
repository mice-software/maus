#!/usr/bin/env python
import io
from InputPyJSON import *
from OutputPyJSON import *
from MapCppSimulation import *
from MapPyRemoveTracks import *
from ReducePyDoNothing import *
from MapPyGroup import *
from MapCppPrint import *
from MapCppTOFDigitization import *
from Go import *

import os, sys

# Create an empty array
myMap = MapPyGroup()

#file = '%s/Models/Configurations/Stage6_FieldsWithoutMaterial.dat' % os.environ.get('MICEFILES')
file = '%s/Models/Configurations/Stage6.dat' % os.environ.get('MICEFILES')

print(file)
# Append the mappers we want
sim = MapCppSimulation()
sim.DisableStoredTracks()
myMap.append(sim)
rt = MapPyRemoveTracks()
#rt.KeepOnlyMuons()
myMap.append(rt)
tof = MapCppTOFDigitization()
myMap.append(tof)

#myMap.append(rt)

#Stage1.dat  Stage2.dat  Stage3.dat  Stage4.dat  Stage5.dat  Stage6.dat  Stage6_FieldsWithoutMaterial.dat
#'%s/Models/Configurations/Stage6_FieldsWithoutMaterial.dat' % os.environ.get('MICEFILES')
print(sys.argv)
document = io.StringIO(int(sys.argv[1])*u"""{"mc": [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210, "random_seed" : 10, "unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n""")


Go(InputPyJSON(document), myMap, ReducePyDoNothing(), OutputPyJSON())

#new_document = map(myMap.Process, [document])
#print new_document
#new_document = reduce(ReducePyDoNothing.Process, [document])
#print new_document



#f = open("output", "w")
#for thing in new_document:
#    f.write("%s" % thing)
#f.close()
