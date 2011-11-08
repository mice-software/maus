"""Tests for src/common_py/Go.py"""

import unittest
import sys
import copy
import tempfile
from io import StringIO

from InputPyEmptyDocument import InputPyEmptyDocument
from MapPyDoNothing import MapPyDoNothing
from ReducePyDoNothing import ReducePyDoNothing
from OutputPyJSON import OutputPyJSON

from Go import Go, get_possible_dataflows

class FakeMap(): #pylint: disable = W0232, R0903
    """Map mock-up that always fails to birth"""
    def birth(self, some_arg): #pylint: disable = W0613, R0201, R0903
        """Always return False"""
        return False

class GoTestCase(unittest.TestCase): #pylint: disable = R0904
    """Tests for src/common_py/Go.py"""

    def setUp(self): #pylint: disable = C0103
        """Create temp file"""
        self.tmp_file = tempfile.mkstemp()[1]

    def dataflows_test(self):
        """
        Make sure get_possible_dataflows() doesn't return nonsense
        """
        keys = get_possible_dataflows().keys()
        self.assertTrue('pipeline_single_thread' in keys)

    def input_birth_test(self):
        """Check that Go raises error with bad input"""
        inputer = FakeMap()
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, command_line_args=False)


    def map_birth_test(self):
        """Check that Go raises error with bad transformer"""
        inputer = InputPyEmptyDocument(1)
        transformer = FakeMap()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, command_line_args=False)

    def reduce_birth_test(self):
        """Check that Go raises error with bad merger"""
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = FakeMap()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, command_line_args=False)

    def output_birth_test(self):
        """Check that Go raises error with bad outputter"""
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = FakeMap()

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, command_line_args=False)

    def command_line_args_test(self):
        """Check that Go handles command line args switch correctly"""
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0]]
    
        Go(inputer, transformer, merger, outputer, command_line_args = True)

        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0], "bob"]
        with self.assertRaises(SystemExit):
            Go(inputer, transformer, merger, outputer, command_line_args = True)

        sys.argv = [arg_temp[0], "-verbose_level", "1"]
        Go(inputer, transformer, merger, outputer, command_line_args = True)

        sys.argv = arg_temp

    def test_type_of_dataflow_bad(self):
        """Check that Go raises error with bad dataflow type"""
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        config = StringIO(u"""type_of_dataflow="bad_type" """)

        with self.assertRaises(LookupError):
            Go(inputer, transformer, merger, outputer, config, \
               command_line_args = False)


    def test_dataflow_single_thread(self):
        """
        Check that Go executes okay with pipeline_single_thread dataflow.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        for map_red_type in ['pipeline_single_thread']:
            config = StringIO(u"""type_of_dataflow="%s" """ % map_red_type)

            Go(inputer, transformer, merger, outputer, config, \
                   command_line_args = False)

    def test_dataflow_multi_process(self):
        """
        Check that Go executes okay with multi_process dataflow.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        for map_red_type in ['multi_process']:
            config = StringIO(u"""type_of_dataflow="%s" """ % map_red_type)

            with self.assertRaises(Exception):
                Go(inputer, transformer, merger, outputer, config, \
                       command_line_args = False)

    def test_dataflow_not_implemented(self):
        """Check that Go notifies user of unimplemented dataflow"""
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        for map_red_type in ['many_local_threads']:
            config = StringIO(u"""type_of_dataflow="%s" """ % map_red_type)
            
            with self.assertRaises(NotImplementedError):
                Go(inputer, transformer, merger, outputer, config, \
                       command_line_args = False)

if __name__ == '__main__':
    unittest.main()
