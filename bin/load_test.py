#!/usr/bin/env python
import sys

from InputPyEmptyDocument import *
from MapPyGroup import *
from MapPyFakeTestSimulation import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from OutputPyDoNothing import *

from Go import *

mapGroup = MapPyGroup()

big_number = 1000

if len(sys.argv) == 3:
    print 'do nothing'
    mapGroup.append(MapPyDoNothing())
else:
    mapGroup.append(MapPyFakeTestSimulation())

if len(sys.argv) >= 2:
    big_number = int(sys.argv[1])


print("Testing with %d events..." % big_number)


inputer = InputPyEmptyDocument(big_number)
# mapMapGroup
reducer = ReducePyDoNothing()
outputer = OutputPyDoNothing()


Go(inputer, mapGroup, reducer, outputer)
