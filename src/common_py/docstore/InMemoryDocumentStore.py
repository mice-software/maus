"""
In-memory document store.
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

class InMemoryDocumentStore:
    """
    In-memory document store.
    """

    def __init__(self):
        """ 
        Constructor.
        @param self Object reference.
        """
        self.__data_store = {}

    def connect(self, parameters = None):
        """ 
        Connect to the data store - this is a no-op.
        @param self Object reference.
        @param parameters Connection information.
        """
        pass

    def ids(self):
        """ 
        Get a list of IDs of the documents in the data store.
        @param self Object reference.
        @return ID list.
        """
        return self.__data_store.keys()

    def __len__(self):
        """ 
        Get number of documents in the data store.
        @param self Object reference.
        @return number >= 0.
        """
        return len(self.__data_store.keys())

    def put(self, docid, doc):
        """ 
        Put a document with the given ID into the data store. Any existing
        document with the same ID is overwritten.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        self.__data_store[docid] = doc

    def get(self, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param docid Document ID.
        @return document or None.
        """
        if self.__data_store.has_key(docid):
            return self.__data_store.get(docid)
        else:
            return None

    def delete(self, docid):
        """ 
        Delete the document with the given ID from the data store.
        If there is no such document then this is a no-op.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        if self.__data_store.has_key(docid):
            self.__data_store.pop(docid)

    def clear(self):
        """ 
        Clear all the documents from the data store.
        @param self Object reference.
        """
        self.__data_store.clear()

