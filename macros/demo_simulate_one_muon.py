#!/usr/bin/env python
from MapCppSimulation import *
from MapPyRemoveTracks import *
from Map import * # required by MAUS


# Create an empty array
myMap = Map()

# Append the mappers we want
myMap.append(MapCppSimulation())
myMap.append(MapPyRemoveTracks())

document = """{
"mc": [{
"position": { "x": 0.1, "y": -0.1, "z": -5000 },
"particle_id" : 13,
"energy" : 210,
"unit_momentum" : { "x":0, "y":0, "z":1 }
}]
}"""

myMap.Birth()
new_document = map(myMap.Process, [document])

for thing in new_document:
    f = open("output", "w")
    f.write("%s\n" % thing)
    f.close()
