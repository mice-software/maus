#!/usr/bin/env python
import sys

from InputPyEmptyDocument import *
from MapPyFakeTestSimulation import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from OutputPyJSON import *

from Go import *

big_number = 1000
if len(sys.argv) == 2:
    big_number = int(sys.argv[1])

print "Testing with %d events..." % big_number

inputer = InputPyEmptyDocument(big_number)
mapper = MapPyFakeTestSimulation()
#mapper = MapPyDoNothing()
reducer = ReducePyDoNothing()
outputer = OutputPyJSON('load_test')


Go(inputer, mapper, reducer, outputer, argKeepTemp=True)
