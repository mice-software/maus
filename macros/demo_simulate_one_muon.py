#!/usr/bin/env python
from MapCppSimulation import MapCppSimulation
from MapPyRemoveTracks import MapPyRemoveTracks


mappers = [MapCppSimulation(), MapPyRemoveTracks(),]

mappers[0].Birth()
document = """{
"mc": [{
"position": { "x": 0.1, "y": -0.1, "z": -5000 },
"particle_id" : 13,
"energy" : 210,
"unit_momentum" : { "x":0, "y":0, "z":1 }
}]
}"""

map(mappers[0].Process, [document])

mappers[0].Death()

