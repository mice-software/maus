"""Tests for src/common_py/Go.py"""

import json
import unittest
import subprocess
import glob
import os
import sys
import copy
from io import StringIO

from InputPyEmptyDocument import InputPyEmptyDocument
from MapPyDoNothing import MapPyDoNothing
from ReducePyDoNothing import ReducePyDoNothing
from OutputPyJSON import OutputPyJSON

from Go import Go

class FakeMap(): #pylint: disable = W0232, R0903
    """Map mock-up that always fails to birth"""
    def birth(self, some_arg): #pylint: disable = W0613, R0201, R0903
        """Always return False"""
        return False

class GoTestCase(unittest.TestCase): #pylint: disable = R0904
    """Tests for src/common_py/Go.py"""
    def input_birth_test(self):
        """Check that Go raises error with bad input"""
        inputer = FakeMap()
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, command_line_args = False)


    def map_birth_test(self):
        """Check that Go raises error with bad mapper"""
        inputer = InputPyEmptyDocument(1)
        mapper = FakeMap()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, command_line_args = False)

    def reduce_birth_test(self):
        """Check that Go raises error with bad reducer"""
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = FakeMap()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, command_line_args = False)

    def output_birth_test(self):
        """Check that Go raises error with bad outputter"""
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = FakeMap()

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, command_line_args = False)

    def command_line_args_test(self):
        """Check that Go handles command line args switch correctly"""
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0]]
    
        Go(inputer, mapper, reducer, outputer, command_line_args = True)

        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0], "bob"]
        with self.assertRaises(SystemExit):
            Go(inputer, mapper, reducer, outputer, command_line_args = True)

        sys.argv = [arg_temp[0], "-verbose_level", "1"]
        Go(inputer, mapper, reducer, outputer, command_line_args = True)

        sys.argv = arg_temp

    def test_map_reduce_type_bad(self):
        """Check that Go raises error with bad map reduce type"""
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        config = StringIO(u"""map_reduce_type="bad_type" """)

        with self.assertRaises(AssertionError):
            Go(inputer, mapper, reducer, outputer, config, \
               command_line_args = False)


    def test_map_reduce_type_good(self):
        """Check that Go executes okay with good map reduce"""
        inputer = InputPyEmptyDocument(1)
        mapper = MapPyDoNothing()
        reducer = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        for map_red_type in ["native_python", "native_python_profile"]:
            config = StringIO(u"""map_reduce_type="%s" """ % map_red_type)

        Go(inputer, mapper, reducer, outputer, config, \
           command_line_args = False)

    tmp_file = os.path.join(os.getenv("MAUS_ROOT_DIR"), "tmp", \
                            "test_core_go.tmp")



if __name__ == '__main__':
    unittest.main()
