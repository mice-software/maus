"""
Tests for MongoDBDocumentStore module.
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

import pymongo
from pymongo.errors import AutoReconnect
import unittest

from docstore.MongoDBDocumentStore import MongoDBDocumentStore

class MongoDBDocumentStoreTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for MongoDBDocumentStore module.
    """

    def setUp(self):
        """ 
        Create MongoDBDocumentStore with test-specific database.
        @param self Object reference.
        """
        self._host = "localhost"
        self._port = 27017
        try:
            test_conx = pymongo.Connection(self._host, self._port)
        except AutoReconnect: # pylint: disable=W0702
            unittest.TestCase.skipTest(self, 
                                       "MongoDB server is not accessible")
        test_conx.disconnect()
        # Create data store and connect.
        self._database_name = self.__class__.__name__
        self._collection_name = self.__class__.__name__
        self._data_store = MongoDBDocumentStore()
        parameters = {
            "mongodb_host":self._host,
            "mongodb_port":self._port,
            "mongodb_database_name":self._database_name,
            "mongodb_collection_name":self._collection_name}
        self._data_store.connect(parameters)

    def tearDown(self):
        """
        Delete test-specific database.
        @param self Object reference.
        """
        self._data_store.disconnect()
        server = pymongo.Connection(self._host, self._port)
        server.drop_database(self._database_name)
        server.disconnect()

    def test_connect_no_parameters(self):
        """
        Test connect with no parameters throws a KeyError as a host
        is expected.
        @param self Object reference.
        """
        try:
            self._data_store.connect({})
        except KeyError:
            pass

    def test_connect_bad_host(self):
        """
        Test connect with a bad host throws an AutoReconnect error as
        expected. 
        @param self Object reference.
        """
        parameters = {"mongodb_host":"nonExistant", "mongodb_port":999999}
        try:
            self._data_store.connect(parameters)
        except AutoReconnect:
            pass

    def test_connect_bad_port(self):
        """
        Test connect with a bad port throws an AutoReconnect error as
        expected. 
        @param self Object reference.
        """
        parameters = {"mongodb_host":self._host, "mongodb_port":999999}
        try:
            self._data_store.connect(parameters)
        except AutoReconnect:
            pass

    def test_connect_no_database_name(self):
        """
        Test connect with no database name parameter throws a KeyError.
        @param self Object reference.
        """
        parameters = {
            "mongodb_host":self._host,
            "mongodb_port":self._port}
        try:
            self._data_store.connect(parameters)
        except KeyError:
            pass

    def test_connect_no_collection_name(self):
        """
        Test connect with no collection name parameter throws a KeyError.
        @param self Object reference.
        """
        parameters = {
            "mongodb_host":self._host,
            "mongodb_port":self._port,
            "mongodb_database_name":self._database_name}
        try:
            self._data_store.connect(parameters)
        except KeyError:
            pass

    def test_empty_data_store(self):
        """
        Test get, delete, clear, len, ids on an empty data store.
        @param self Object reference.
        """
        self.assertEquals(0, len(self._data_store),
            "Unexpected len")
        self.assertEquals(0, len(self._data_store.ids()),
            "Unexpected number of IDs")
        self.assertEquals(None, self._data_store.get("ID"),
            "Expected document to be None")
        # Expect no exceptions.
        self._data_store.delete("ID")
        # Expect no exceptions.
        self._data_store.clear()

    def test_put_get(self):
        """
        Test put and get, and also ids and len.
        @param self Object reference.
        """
        # Insert documents.
        doc1 = {'a1':'b1', 'c1':'d1'}
        self._data_store.put("ID1", doc1)
        doc2 = {'a2':'b2', 'c2':'d2'}
        self._data_store.put("ID2", doc2)
        # Validate.
        self.assertEquals(2, len(self._data_store),
            "Unexpected len")
        ids = self._data_store.ids()
        self.assertEquals(2, len(ids), "Unexpected number of IDs")
        self.assertTrue("ID1" in ids, "ID1 was not in ids")
        self.assertTrue("ID2" in ids, "ID2 was not in ids")
        self.assertEquals(doc1, self._data_store.get("ID1"),
            "Unexpected document for ID1")
        self.assertEquals(doc2, self._data_store.get("ID2"),
            "Unexpected document for ID2")

    def test_put_put(self):
        """
        Test put then another with the same ID.
        @param self Object reference.
        """
        # Insert document.
        doc = {'a1':'b1', 'c1':'d1'}
        self._data_store.put("ID", doc)
        self.assertEquals(doc, self._data_store.get("ID"),
            "Unexpected document for ID")
        # Overwrite
        nudoc = {'a2':'b2', 'c2':'d2'}
        self._data_store.put("ID", nudoc)
        # Validate.
        self.assertEquals(nudoc, self._data_store.get("ID"),
            "Unexpected document for ID")

    def test_delete(self):
        """
        Test delete.
        @param self Object reference.
        """
        self._data_store.put("ID1", {'a1':'b1', 'c1':'d1'})
        self._data_store.put("ID2", {'a2':'b2', 'c2':'d2'})

        self._data_store.delete("ID1")
        self.assertEquals(1, len(self._data_store),
            "Unexpected len")
        ids = self._data_store.ids()
        self.assertEquals(1, len(ids), "Unexpected number of IDs")
        self.assertTrue(not "ID1" in ids, "ID1 was not in ids")

        self._data_store.delete("ID2")
        self.assertEquals(0, len(self._data_store),
            "Unexpected len")
        ids = self._data_store.ids()
        self.assertEquals(0, len(ids), "Unexpected number of IDs")

    def test_clear(self):
        """
        Test clear.
        @param self Object reference.
        """
        self._data_store.put("ID1", {'a1':'b1', 'c1':'d1'})
        self._data_store.put("ID2", {'a2':'b2', 'c2':'d2'})
        self._data_store.clear()
        self.assertEquals(0, len(self._data_store),
            "Unexpected len")
        self.assertEquals(0, len(self._data_store.ids()), 
            "Unexpected number of IDs")

if __name__ == '__main__':
    unittest.main()
