#!/usr/bin/env python
import StringIO
from InputPyJSON import *
from MapCppSimulation import *
from MapPyRemoveTracks import *
from ReducePyDoNothing import *
from Map import * # required by MAUS
from Go import *

# Create an empty array
myMap = Map()

# Append the mappers we want
myMap.append(MapCppSimulation())
rt = MapPyRemoveTracks()
rt.KeepOnlyMuons()
myMap.append(rt)

document = StringIO.StringIO("""{
"mc": [{
"position": { "x": 0.1, "y": -0.1, "z": -5000 },
"particle_id" : 13,
"energy" : 210,
"unit_momentum" : { "x":0, "y":0, "z":1 }
}]
}""")


Go(InputPyJSON(document), myMap, ReducePyDoNothing, None)

#new_document = map(myMap.Process, [document])
#print new_document
#new_document = reduce(ReducePyDoNothing.Process, [document])
#print new_document



#f = open("output", "w")
#for thing in new_document:
#    f.write("%s" % thing)
#f.close()
