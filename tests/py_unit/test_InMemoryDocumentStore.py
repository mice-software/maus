"""
Tests for InMemoryDocumentStore module.
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

class InMemoryDocumentStoreTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for InMemoryDocumentStore module.
    """

    def setUp(self):
        """ 
        Invoke "birth" and check for success.
        @param self Object reference.
        """
        self._data_store = InMemoryDocumentStore()
        self._data_store.connect({})

    def test_connect_no_parameters(self):
        """
        Test connect with no parameters.
        @param self Object reference.
        """
        # Expect no exceptions.
        self._data_store.connect()

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
