"""
Document store super-class.
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

class DocumentStore: # pylint: disable=W0232
    """
    Document store super-class which holds documents identified by
    a unique identifier and timestamped with their time added to
    the store.
    """

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
        pass

    def __len__(self):
        """ 
        Get number of documents in the data store.
        @param self Object reference.
        @return number >= 0.
        """
        pass

    def put(self, docid, doc):
        """ 
        Put a document with the given ID into the data store. Any existing
        document with the same ID is overwritten. The time of addition
        is also recorded.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        pass

    def get(self, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param docid Document ID.
        @return document or None.
        """
        pass

    def get_since(self, earliest):
        """ 
        Get the documents added since the given date from the data 
        store or None if there is none.
        @param self Object reference.
        @param earliest datetime representing date of interest. If
        None then all are returned. 
        @return iterable serving up the documents in the form
        {'_id':id, 'date':date, 'doc':doc} where date is in the
        Python datetime format e.g. YYYY-MM-DD HH:MM:SS.MILLIS.
        Documents are sorted earliest to latest.
        """
        pass

    def delete(self, docid):
        """ 
        Delete the document with the given ID from the data store.
        If there is no such document then this is a no-op.
        @param self Object reference.
        @param docid Document ID.
        """
        pass

    def clear(self):
        """ 
        Clear all the documents from the data store.
        @param self Object reference.
        """
        pass

    def disconnect(self):
        """
        Disconnect. If there is no notion of disconnect this is a no-op.
        @param self Object reference.
        """
        pass
