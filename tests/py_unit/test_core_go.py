""" Tests for src/common_py/Go.py """

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

import unittest
import sys
import copy
import tempfile
from io import StringIO

from InputPyEmptyDocument import InputPyEmptyDocument
from MapPyDoNothing import MapPyDoNothing
from ReducePyDoNothing import ReducePyDoNothing
from OutputPyJSON import OutputPyJSON

from Go import Go

CONFIG = """
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
"""


class FakeWorker(): #pylint: disable = W0232, R0903
    """ Mock worker that always fails to birth. """

    def birth(self, some_arg): #pylint: disable = W0613, R0201, R0903
        """  
        Birth function that always returns False.
        @param self. Object reference.
        @param some_arg Ignored.
        @return False always.
        """
        return False

class GoTestCase(unittest.TestCase): #pylint: disable = R0904
    """ Tests for src/common_py/Go.py. """

    def setUp(self): #pylint: disable = C0103
        """  
        Create temporary file.
        @param self. Object reference.
        """
        self.tmp_file = tempfile.mkstemp()[1]
        self.config = StringIO(unicode(CONFIG))

    def test_dataflows(self):
        """  
        Make sure get_possible_dataflows() doesn't return nonsense
        @param self. Object reference.
        """
        keys = Go.get_possible_dataflows().keys()
        self.assertTrue('pipeline_single_thread' in keys)

    def test_input_birth(self):
        """
        Check that Go raises error with bad input.
        @param self. Object reference.
        """
        inputer = FakeWorker()
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, \
                              config_file=self.config, command_line_args=False)

    def test_map_birth(self):
        """
        Check that Go raises error with bad transformer.
        @param self. Object reference.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = FakeWorker()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, \
                               config_file=self.config, command_line_args=False)

    def test_reduce_birth(self):
        """
        Check that Go raises error with bad merger.
        @param self. Object reference.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = FakeWorker()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, \
                             config_file=self.config, command_line_args=False)

    def test_output_birth(self):
        """
        Check that Go raises error with bad outputter.
        @param self. Object reference.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = FakeWorker()

        with self.assertRaises(AssertionError):
            Go(inputer, transformer, merger, outputer, \
                               config_file=self.config, command_line_args=False)

    def test_command_line_args(self):
        """
        Check that Go handles command line args switch correctly.
        @param self. Object reference.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0]]
    
        Go(inputer, transformer, merger, outputer, config_file=self.config, \
                                                      command_line_args = True)

        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        arg_temp = copy.deepcopy(sys.argv)
        sys.argv = [arg_temp[0], "bob"]
        with self.assertRaises(SystemExit):
            Go(inputer, transformer, merger, outputer, \
                              config_file=self.config, command_line_args = True)

        sys.argv = [arg_temp[0], "-verbose_level", "1"]
        Go(inputer, transformer, merger, outputer, \
                             config_file=self.config, command_line_args = True)

        sys.argv = arg_temp

    def test_type_of_dataflow_bad(self):
        """
        Check that Go raises error with bad dataflow type.
        @param self. Object reference.
        """
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
        @param self. Object reference.
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
        @param self. Object reference.
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
        """
        Check that Go notifies user of unimplemented dataflow.
        @param self. Object reference.
        """
        inputer = InputPyEmptyDocument(1)
        transformer = MapPyDoNothing()
        merger = ReducePyDoNothing()
        outputer = OutputPyJSON(open(self.tmp_file, 'w'))
        for map_red_type in ['many_local_threads']:
            config = StringIO(u"""type_of_dataflow="%s" """ % map_red_type)
            
            with self.assertRaises(NotImplementedError):
                Go(inputer, transformer, merger, outputer, config, \
                       command_line_args = False)

    def test_initialise(self):
        """
        Check that initialisation of MAUS is okay
        """
        pass

if __name__ == '__main__':
    unittest.main()
