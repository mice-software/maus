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

from datetime import datetime
import time
from docstore.DocumentStore import DocumentStore

class InMemoryDocumentStore(DocumentStore):
    """
    In-memory document store.

    We store data in two columns - 'data' is a list containing items one for
    each entry; 'hash' is a map from id to data entry. This structure allows
    fast hashing lookups by id and also max_size (removing first item from the
    list)    
    """

    def __init__(self):
        """ 
        Constructor.
        @param self Object reference.
        """
        self.__data_store = {}

    def collection_names(self):
        """ 
        Get a list of collection names.
        @param self Object reference.
        @return list.
        """
        return self.__data_store.keys()

    def create_collection(self, collection, max_number_of_documents):
        """ 
        Create a collection. If it already exists, this is a no-op.
        @param self Object reference.
        @param collection Collection name.
        @param max_number_of_documents Limit the collection to hold at most this
               number of documents.
        """
        if (not self.__data_store.has_key(collection)):
            self.__data_store[collection] = {}
            self.__data_store[collection]['data'] = []
            self.__data_store[collection]['hash'] = {}
            self.__data_store[collection]['max_docs'] = max_number_of_documents

    def has_collection(self, collection):
        """ 
        Check if collection exists.
        @param self Object reference.
        @param collection Collection name.
        @return True if collection exists else False.
        """
        return self.__data_store.has_key(collection)

    def get_ids(self, collection):
        """ 
        Get a list of IDs of the documents in the collection.
        @param self Object reference.
        @param collection Collection name.
        @return ID list.
        """
        return self.__data_store[collection]['hash'].keys()

    def count(self, collection):
        """ 
        Get number of documents in the collection.
        @param self Object reference.
        @param collection Collection name.
        @return number >= 0.
        """
        return len(self.__data_store[collection]['data'])

    def put(self, collection, docid, doc):
        """ 
        Put a document with the given ID into the data store. Any existing
        document with the same ID is overwritten. The time of addition
        is also recorded.
        @param self Object reference.
        @param collection Collection name.
        @param docid Document ID.
        @param doc Document.
        """
        # Get (YYYY,MM,DD,HH,MM,SS,MILLI)
        current_time = datetime.fromtimestamp(time.time())
        _hash = self.__data_store[collection]['hash']
        _data = self.__data_store[collection]['data']
        _data.append((doc, current_time, docid))
        _hash[docid] = _data[-1]
        if len(_data) > self.__data_store[collection]['max_docs']:
            del _hash[_data[0][2]] 
            self.__data_store[collection]['data'] = _data[1:]

    def get(self, collection, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param collection Collection name.
        @param docid Document ID.
        @return document or None.
        """
        if self.__data_store[collection]['hash'].has_key(docid):
            return self.__data_store[collection]['hash'][docid][0]
        else:
            return None

    def get_since(self, collection_name, earliest = None):
        """ 
        Get the documents added since the given date from the data 
        store or None if there is none.
        @param self Object reference.
        @param collection Collection name.
        @param earliest datetime representing date of interest. If
        None then all are returned.
        @return iterable serving up the documents in the form
        {'_id':id, 'date':date, 'doc':doc} where date is in the
        Python datetime format e.g. YYYY-MM-DD HH:MM:SS.MILLIS.
        Documents are sorted earliest to latest.
        """
        since = []
        collection = self.__data_store[collection_name]
        if (earliest == None):
            for (docid, doc) in collection['hash'].items():
                since.append({'_id':docid, 'date':doc[1], 'doc':doc[0]})
        else:
            for (docid, doc) in collection['hash'].items():
                if (doc[1] > earliest):
                    since.append({'_id':docid, 'date':doc[1], 'doc':doc[0]})
        sorted_since = sorted(since, key=lambda item: item['date'])
        return iter(sorted_since)

    def delete_collection(self, collection):
        """ 
        Delete collection.
        @param self Object reference.
        @param collection Collection name.
        """
        self.__data_store.pop(collection)
