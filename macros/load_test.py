#!/usr/bin/env python
import sys
import gzip

from InputPyEmptyDocument import *
from MapPyFakeTestSimulation import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from OutputPyJSON import *
import io
from Go import *

big_number = 1000
use_tracks = False
if len(sys.argv) == 2:
    big_number = int(sys.argv[1])
    print big_number
    if big_number < 0:
        myConfig = io.StringIO(u"MapPyFakeTestSimulation_withTracks = False")
        big_number = -1 * big_number
    else:
        myConfig = io.StringIO(u"MapPyFakeTestSimulation_withTracks = True")


print("Testing with %d events..." % big_number)

inputer = InputPyEmptyDocument(big_number)
mapper = MapPyFakeTestSimulation()

#mapper = MapPyDoNothing()
reducer = ReducePyDoNothing()
outputer = OutputPyJSON(gzip.GzipFile('mausput.gz', 'wb'))


Go(inputer, mapper, reducer, outputer, myConfig)
