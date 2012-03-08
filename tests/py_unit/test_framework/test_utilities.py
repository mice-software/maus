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

from docstore.DocumentStore import DocumentStore
from docstore.DocumentStore import DocumentStoreException
from docstore.InMemoryDocumentStore import InMemoryDocumentStore
from framework.utilities import CeleryUtilities
from framework.utilities import CeleryNodeException
from framework.utilities import DataflowUtilities
from framework.utilities import DocumentStoreUtilities
from framework.utilities import NoCeleryNodeException
from framework.utilities import RabbitMQException

class DataflowUtilitiesTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.DataflowUtilities.
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
        self.assertEquals(None,
            DataflowUtilities.get_run_number({}),
            "Unexpected run number")

class DocumentStoreUtilitiesTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.DocumentStoreUtilities.
    """

    def test_setup_doc_store_no_doc_store_class(self):
        """
        Test with no "doc_store_class" entry in JSON configuration.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(KeyError,
            ".*"):
            DocumentStoreUtilities.setup_doc_store({})

    def test_setup_doc_store_no_module(self):
        """
        Test where "doc_store_class" specifies a class but
        no module prefix.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = "ClassOnly"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DocumentStoreUtilities.setup_doc_store(config)

    def test_setup_doc_store_bad_module(self):
        """
        Test where "doc_store_class" specifies a non-existant
        module.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = "nonexistant.NonExistantModule"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DocumentStoreUtilities.setup_doc_store(config)

    def test_setup_doc_store_bad_class(self):
        """
        Test where "doc_store_class" specifies a non-existant
        class.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "docstore.InMemoryDocumentStore.NonExistantClass"
        with self.assertRaisesRegexp(ValueError, ".*"):
            DocumentStoreUtilities.setup_doc_store(config)

    def test_setup_doc_store_not_subclass(self):
        """
        Test where "doc_store_class" specifies a class that does
        not subclass docstore.DocumentStore.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "framework.utilities.DocumentStoreUtilities"
        with self.assertRaisesRegexp(TypeError,
            ".*"):
            DocumentStoreUtilities.setup_doc_store(config)

    def test_setup_doc_store_connect_fails(self):
        """
        Test where "doc_store_class" specifies a class whose
        connect method throws an exception.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "test_framework.test_utilities.MockDocumentStore"
        config["fail"] = "fail"
        with self.assertRaisesRegexp(DocumentStoreException,
            ".*"):
            DocumentStoreUtilities.setup_doc_store(config)

    def test_setup_doc_store(self):
        """
        Test where arguments are valid.
        @param self Object reference.
        """
        config = {}
        config["doc_store_class"] = \
            "docstore.InMemoryDocumentStore.InMemoryDocumentStore"
        doc_store = DocumentStoreUtilities.setup_doc_store(config)
        self.assertTrue(isinstance(doc_store, InMemoryDocumentStore),
            "Document store isn't instance of given class")

    def test_create_doc_store_collection(self):
        """
        Test where arguments are valid.
        @param self Object reference.
        """
        doc_store = InMemoryDocumentStore()
        doc_store.create_collection("test")
        doc_store.put("test", "1", {})
        doc_store.put("test", "2", {})
        self.assertEquals(2, doc_store.count("test"), 
            "Expected 2 documents")
        DocumentStoreUtilities.create_doc_store_collection(doc_store,
            "test")
        # Collection should have been deleted and recreated.
        self.assertEquals(0, doc_store.count("test"), 
            "Expected 0 documents")

    def test_create_doc_store_collection_exception(self):
        """
        Test where document store throws an exception.
        @param self Object reference.
        """
        doc_store = MockDocumentStore()
        with self.assertRaisesRegexp(DocumentStoreException,
            ".*"):
            DocumentStoreUtilities.create_doc_store_collection( \
                doc_store, "fail")

class MockDocumentStore(DocumentStore): 
    """
    Mock DocumentStore whose methods can throw exceptions.
    """

    def __init__(self):
        """
        No-op.
        @param self Object reference.
        """
        pass

    def connect(self, parameters = None):
        """ 
        Connect to the document store.
        @param self Object reference.
        @param parameters Connection information.
        @throws ValueException if there is a key "fail"
        """
        if (parameters != None) and (parameters.has_key("fail")):
            raise ValueError("Test")

    def has_collection(self, collection):
        """ 
        Check if collection exists.
        @param self Object reference.
        @param collection Collection name.
        @return True unless collection is "fail".
        @throws ValueError if collection is "fail"
        """
        if (collection == "fail"):
            raise ValueError("Test")

class CeleryUtilitiesTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.CeleryUtilities.
    """

    def test_validate_celery_response_empty(self):
        """
        Test with empty list of node statuses.
        @param self Object reference.
        """
        CeleryUtilities.validate_celery_response([])
        self.assertTrue(True, "Test passed")

    def test_validate_celery_response(self):
        """
        Test with list of OK node statuses.
        @param self Object reference.
        """
        node1 = {"node1": {"status":"ok"}}
        node2 = {"node2": {"status":"ok"}}
        node3 = {"node3": {"status":"ok"}}
        response = [node1, node2, node3]
        CeleryUtilities.validate_celery_response(response)
        self.assertTrue(True, "Test passed")

    def test_validate_celery_response_exception(self):
        """
        Test with list mixing OK and error statuses.
        @param self Object reference.
        """
        node1 = {"node1": {"status":"ok"}}
        node2 = {"node2": {"status":"error", \
            "error":[{"type":"ERROR2", "message":"MESSAGE2"}]}}
        node3 = {"node3": {"status":"ok"}}
        node4 = {"node4": {"status":"error", \
            "error":[{"type":"ERROR4", "message":"MESSAGE4"}]}}
        response = [node1, node2, node3, node4]
        try:
            CeleryUtilities.validate_celery_response(response)
            self.fail("Expected CeleryNodeException")    
        except CeleryNodeException as exc:
            nodes = exc.node_status
            self.assertEquals(2, len(nodes), 
                "Unexpected number of failed nodes")
            self.assertEquals(nodes[0], ("node2", node2["node2"]),
                "Unexpected value for nodes[0]")
            self.assertEquals(nodes[1], ("node4", node4["node4"]),
                "Unexpected value for nodes[1]")
        except:
            self.fail("Expected CeleryNodeException")    

class RabbitMQExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.RabbitMQException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        value_error = ValueError("Test")
        exception = RabbitMQException(value_error)
        self.assertEquals("RabbitMQ cannot be contacted. Problem is Test", 
            str(exception), "Unexpected string")
        self.assertEquals(value_error, exception.exception,
            "Unexpected exception")

class NoCeleryNodeExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.NoCeleryNodeException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = NoCeleryNodeException()
        self.assertEquals("No Celery nodes are available", 
            str(exception), "Unexpected string")

class CeleryNodeExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.utilities.CeleryNodeException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = CeleryNodeException()
        self.assertEquals("Celery node(s) [] failed to configure", 
            str(exception), "Unexpected string")
        self.assertEquals([], exception.node_status,
            "Unexpected node status")

    def test_init_node_status(self):
        """ 
        Invoke constructor with some node_status and check values.
        @param self Object reference.
        """
        node_status = [("1", "Failure 1"), ("2", "Failure 2")]
        exception = CeleryNodeException(node_status)
        expected = "Celery node(s) %s failed to configure" % ["1", "2"]
        self.assertEquals(expected, str(exception), "Unexpected string")
        self.assertEquals(node_status, exception.node_status,
            "Unexpected node status")

if __name__ == '__main__':
    unittest.main()
