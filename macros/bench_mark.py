#!/usr/bin/env python
import io
from InputPyJSON import *
from MapCppSimulation import *
from MapPyRemoveTracks import *
from ReducePyDoNothing import *
from MapPyGroup import *
from Go import *

import sys
import json

print(sys.argv)
assert len(sys.argv) == 3

number_spills = int(sys.argv[1])
number_particles_per_spill = int(sys.argv[2])

print("Running %d spills with %d particles per spill...\n" % (number_spills, number_particles_per_spill))

# Create an empty array
myMap = MapPyGroup()

# Append the mappers we want
myMap.append(MapCppSimulation())

spill = {}
spill['mc'] = []

particle = {}
particle["position"] = { "x": 0.0, "y": -0.0, "z": -5000 }
particle["particle_id"] = 13
particle["energy"] = 210
particle["unit_momentum"] = { "x":0, "y":0, "z":1}

for i in range(number_particles_per_spill):
    spill['mc'].append(particle)
    
spillString = "%s\n" % json.dumps(spill)

print(spillString)

document = io.StringIO(number_spills * spillString)


Go(InputPyJSON(document), myMap, ReducePyDoNothing, None)

#new_document = map(myMap.Process, [document])
#print new_document
#new_document = reduce(ReducePyDoNothing.Process, [document])
#print new_document



#f = open("output", "w")
#for thing in new_document:
#    f.write("%s" % thing)
#f.close()
