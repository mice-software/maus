#!/usr/bin/env python
import StringIO
from InputPyJSON import *
from MapCppSimulation import *
from MapPyRemoveTracks import *
from ReducePyDoNothing import *
from MapPyGroup import *
from MapCppPrint import *
from Go import *

import os

# Create an empty array
myMap = MapPyGroup()

file = '%s/Models/Configurations/Stage6_FieldsWithoutMaterial.dat' % os.environ.get('MICEFILES')
#file = '%s/Models/Configurations/Stage6.dat' % os.environ.get('MICEFILES')

print file
# Append the mappers we want
myMap.append(MapCppSimulation(file))
rt = MapPyRemoveTracks()
rt.KeepOnlyMuons()
myMap.append(MapCppPrint())
#myMap.append(rt)

#Stage1.dat  Stage2.dat  Stage3.dat  Stage4.dat  Stage5.dat  Stage6.dat  Stage6_FieldsWithoutMaterial.dat
#'%s/Models/Configurations/Stage6_FieldsWithoutMaterial.dat' % os.environ.get('MICEFILES')

document = StringIO.StringIO("""{"mc": [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210,"unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n""")


Go(InputPyJSON(document), myMap, ReducePyDoNothing, None)

#new_document = map(myMap.Process, [document])
#print new_document
#new_document = reduce(ReducePyDoNothing.Process, [document])
#print new_document



#f = open("output", "w")
#for thing in new_document:
#    f.write("%s" % thing)
#f.close()
