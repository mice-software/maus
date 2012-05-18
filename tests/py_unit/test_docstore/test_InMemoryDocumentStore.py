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
from test_docstore.test_DocumentStoreBase import DocumentStoreTests

class InMemoryDocumentStoreTestCase(unittest.TestCase, DocumentStoreTests): # pylint: disable=R0904, C0301
    """
    Test class for InMemoryDocumentStore module. Common tests are
    defined in the DocumentStoreTests mix-in.
    """

    def setUp(self):
        """ 
        Create test object and connect method.
        @param self Object reference.
        """
        unittest.TestCase.setUp(self)
        self._data_store = InMemoryDocumentStore()
        self._data_store.connect({})
        self._collection = self.__class__.__name__
        self._data_store.create_collection(self._collection)

    def tearDown(self):
        """
        Disconnect.
        @param self Object ref
        """
        unittest.TestCase.tearDown(self)
        self._data_store.delete_collection(self._collection)
        self._data_store.disconnect()

    def test_connect_no_parameters(self):
        """
        Test connect with no parameters.
        @param self Object reference.
        """
        # Expect no exceptions.
        self._data_store.connect()

if __name__ == "__main__":
    unittest.main()
