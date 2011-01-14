import json
import unittest
import subprocess
import glob
import os
import sys

from InputPyEmptyDocument import *
from MapPyDoNothing import *
from ReducePyDoNothing import *
from OutputPyJSON import *

from Go import Go

class GoTestCase(unittest.TestCase):
    def test_type_none(self):
        with self.assertRaises(AssertionError):
            Go(None,None,None,None)


    def test_map_reduce_type(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON('unit_test')  #  this file won't appear since exception 

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, argMapReduceType = "bad_type")

        self.assertFalse(os.path.isfile('unit_test'))



    def test_save_temp_type(self):
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON('unit_test')  #  this file won't appear since exception

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, argKeepTemp = "bad_type")

        self.assertFalse(os.path.isfile('unit_test'))



if __name__ == '__main__':
    unittest.main()
