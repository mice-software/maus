"""
Tests for utilities module.
"""

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

# pylint: disable=C0103

import unittest

from docstore.InMemoryDocumentStore import InMemoryDocumentStore
from framework.utilities import DataflowUtilities

class WorkerUtilitiesTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.WorkerUtilities.
    """

    def test_buffer_input_zero(self):
        """
        Test where 0 inputs are requested from an emitter of length
        N.
        @param self Object reference.
        """
        emitter = ["a", "b"].__iter__()
        buff = DataflowUtilities.buffer_input(emitter, 0)
        self.assertEquals([], buff, "Unexpected buffer")

    def test_buffer_input_whole_buffer(self):
        """
        Test buffer input where N inputs are requested from an
        emitter of length N.
        @param self Object reference.
        """
        data = ["a", "b", "c", "d", "e"]
        emitter = data.__iter__()
        buff = DataflowUtilities.buffer_input(emitter, len(data))
        self.assertEquals(data, buff, "Unexpected buffer")

    def test_buffer_input_whole_buffer_plus_one(self):
        """
        Test buffer input where N + 1 inputs are requested from an
        emitter of length N.
        @param self Object reference.
        """
        data = ["a", "b", "c", "d", "e"]
        emitter = data.__iter__()
        buff = DataflowUtilities.buffer_input(emitter, len(data) + 1)
        self.assertEquals(data, buff, "Unexpected buffer")

    def test_setup_doc_store_no_doc_store_class(self):
        """
        Test with no "doc_store_class" entry in JSON configuration.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(KeyError,
            ".*"):
            DataflowUtilities.setup_doc_store({})

    def test_setup_doc_store_no_module(self):
        """
        Test where "doc_store_class" specifies a class but
        no module prefix.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = "ClassOnly"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DataflowUtilities.setup_doc_store(config)

    def test_setup_doc_store_bad_module(self):
        """
        Test where "doc_store_class" specifies a non-existant
        module.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = "nonexistant.NonExistantModule"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DataflowUtilities.setup_doc_store(config)

    def test_setup_doc_store_bad_class(self):
        """
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "docstore.InMemoryDocumentStore.NonExistantClass"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DataflowUtilities.setup_doc_store(config)

    def test_setup_doc_store_not_subclass(self):
        """
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "framework.utilities.DataflowUtilities"
        with self.assertRaisesRegexp(TypeError,
            ".*"):
            DataflowUtilities.setup_doc_store(config)

    def test_setup_doc_store(self):
        """
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "docstore.InMemoryDocumentStore.InMemoryDocumentStore"
        doc_store = DataflowUtilities.setup_doc_store(config)
        self.assertTrue(isinstance(doc_store, InMemoryDocumentStore),
            "Document store isn't instance of given class")

    def test_is_start_of_run_true(self):
        """
        Test with a spill with "daq_event_type":"start_of_run".
        @param self Object reference.
        """
        spill = {}
        spill["daq_event_type"] = "start_of_run"
        self.assertTrue(DataflowUtilities.is_start_of_run(spill),
            "Expected True")

    def test_is_start_of_run_wrong_daq_event(self):
        """
        Test with a spill with "daq_event_type" not equal
        to "start_of_run".
        @param self Object reference.
        """
        spill = {}
        spill["daq_event_type"] = "made_up"
        self.assertTrue(not DataflowUtilities.is_start_of_run(spill),
            "Expected False")

    def test_is_start_of_run_no_daq_event(self):
        """
        Test with a spill no "daq_event_type".
        @param self Object reference.
        """
        self.assertTrue(not DataflowUtilities.is_start_of_run({}),
            "Expected False")

    def test_get_run_number(self):
        """
        Test with a spill that has a "run_num".
        @param self Object reference.
        """
        spill = {}
        spill["run_num"] = 123
        self.assertEquals(123,
            DataflowUtilities.get_run_number(spill),
            "Unexpected run number")

    def test_get_run_number_no_run_num(self):
        """
        Test with a spill that has a no "run_num".
        @param self Object reference.
        """
        self.assertEquals(-1,
            DataflowUtilities.get_run_number({}),
            "Unexpected run number")

if __name__ == '__main__':
    unittest.main()
