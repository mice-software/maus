#!/usr/bin/env python
from MapCppSimulation import *
from MapPyRemoveTracks import *
from Go import * # required by MAUS


# Create an empty array
myMappers = []  

# Append the mappers we want
myMappers.append(MapCppSimulation())
myMappers.append(MapPyRemoveTracks())

goer = Go(mappers=myMappers)

document = """{
"mc": [{
"position": { "x": 0.1, "y": -0.1, "z": -5000 },
"particle_id" : 13,
"energy" : 210,
"unit_momentum" : { "x":0, "y":0, "z":1 }
}]
}"""

new_document = map(goer.Process, [document])

for thing in new_document:
    f = open("output", "w")
    f.write("%s\n" % thing)
    f.close()
