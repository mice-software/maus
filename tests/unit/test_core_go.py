import json
import unittest
import subprocess
import glob
import os
import sys
from io import StringIO

from InputPyEmptyDocument import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from OutputPyJSON import *

from Go import Go

class FakeMap():
    def birth(someArg):
        return False

class GoTestCase(unittest.TestCase):
    def input_birth_test(self):
        inputer = FakeMap()
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open('unit_test', 'w'))  #  this file won't appear since exception
        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer)


    def map_birth_test(self):
        inputer = InputPyEmptyDocument(1)
        mapper = FakeMap
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open('unit_test', 'w'))  #  this file won't appear since exception

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer)

    def reduce_birth_test(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = FakeMap()
        outputer = OutputPyJSON(open('unit_test', 'w'))  #  this file won't appear since exception

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer)

    def output_birth_test(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = FakeMap()

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer)



    def test_map_reduce_type(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open('unit_test', 'w'))  #  this file won't appear since exception

        configFile = StringIO(u"""map_reduce_type="bad_type" """)

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, configFile)


    def test_map_reduce_type(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open('unit_test', 'w'))  #  this file won't appear since exception
        for MapRedType in ["native_python", "native_python_profile"]:
            configFile = StringIO(u"""map_reduce_type="%s" """ % MapRedType)

        Go(inputer, mapper, reducer, outputer, configFile)





if __name__ == '__main__':
    unittest.main()
