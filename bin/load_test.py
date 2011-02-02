#!/usr/bin/env python
import sys
import gzip

from InputPyEmptyDocument import *
from MapPyFakeTestSimulation import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from MapPyRemoveTracks import *
from OutputPyJSON import *
from MapPyGroup import *
import io
from Go import *

group = MapPyGroup()

big_number = 1000

if len(sys.argv) == 3:
    group.append(MapPyDoNothing())
else:
    group.append(MapPyFakeTestSimulation())
    
    if len(sys.argv) >= 2:
        big_number = int(sys.argv[1])

        if big_number < 0:
            rt = MapPyRemoveTracks()                                                                                                                
            group.append(rt)   
            big_number = -1 * big_number

print("Testing with %d events..." % big_number)


inputer = InputPyEmptyDocument(big_number)

#mapper = MapPyDoNothing()
reducer = ReducePyDoNothing()
outputer = OutputPyJSON(gzip.GzipFile('mausput.gz', 'wb'))


Go(inputer, group, reducer, outputer)
