"""
MongoDB-based document store.
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
import pymongo
from pymongo.errors import OperationFailure

from docstore.DocumentStore import DocumentStore
from docstore.DocumentStore import DocumentStoreException

class MongoDBDocumentStore(DocumentStore):
    """
    MongoDB-based document store.
    """

    def __init__(self):
        """ 
        Constructor.
        @param self Object reference.
        """
        self.__mongodb = None
        self.__data_store = None
        
    def connect(self, parameters = None):
        """ 
        Connect to the data store. The connection is configured
        via the following parameters:

        -mongodb_host - MongoDB host name. This is a mandatory parameter.
        -mongodb_port - MongoDB port. This is a mandatory parameter.
        -mongodb_database_name - MongoDB database name. This is a
         mandatory parameter. If the database does not exist then it
         is created.

        @param self Object reference.
        @param parameters Connection information.
        @throws KeyError. If mongodb_host, mongodb_port or
        mongodb_database_name are not provided. 
        @throws DocumentStoreException. If the database cannot
        be contacted using the given host and port.
        """
        try:
            self.__mongodb = pymongo.Connection(
                parameters["mongodb_host"],
                parameters["mongodb_port"])
            self.__data_store = \
                self.__mongodb[parameters["mongodb_database_name"]]
        except pymongo.errors.AutoReconnect as exc:
            raise DocumentStoreException(exc)

    def collection_names(self):
        """ 
        Get a list of collection names.
        @param self Object reference.
        @return list.
        """
        return self.__data_store.collection_names()

    def create_collection(self, collection):
        """ 
        Create a collection. If it already exists, this is a no-op.
        An index is created on the "date" field.
        @param self Object reference.
        @param collection Collection name.
        """
        if (not collection in self.__data_store.collection_names()):
            self.__data_store.create_collection(collection)
            collection = self.__data_store[collection]
            collection.create_index("date")
        
    def has_collection(self, collection):
        """ 
        Check if collection exists.
        @param self Object reference.
        @param collection Collection name.
        @return True if collection exists else False.
        """
        return collection in self.__data_store.collection_names()

    def get_ids(self, collection):
        """ 
        Get a list of IDs of the documents in the collection.
        @param self Object reference.
        @param collection Collection name.
        @return ID list.
        """
        return self.__data_store[collection].distinct("_id")

    def count(self, collection):
        """ 
        Get number of documents in the collection.
        @param self Object reference.
        @param collection Collection name.
        @return number >= 0.
        """
        return self.__data_store[collection].count()

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
        self.__data_store[collection].save( \
            {'_id':docid, 'date':current_time, 'doc':doc})

    def get(self, collection, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param collection Collection name.
        @param docid Document ID.
        @return document or None.
        @throws DocumentStoreException if the operation fails - currently not
                sure under what circumstances operation failure may occur
        """
        try:
            doc = self.__data_store[collection].find_one({"_id":docid},
                                                         timeout=False)
            if doc != None:
                return doc['doc']
            else:
                return None
        except pymongo.errors.OperationFailure as exc:
            raise DocumentStoreException(exc)

    def get_since(self, collection, earliest = None):
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
        @throws DocumentStoreException if the operation fails - currently not
                sure under what circumstances operation failure may occur
        """
        try:
            if (earliest == None):
                result = self.__data_store[collection].find().sort("date")
            else:
                result = self.__data_store[collection].find(\
                                   {"date":{"$gt":earliest}}).sort("date")
            return result
        except pymongo.errors.OperationFailure as exc:
            raise DocumentStoreException(exc)

    def delete_document(self, collection, docid):
        """ 
        Delete the document with the given ID from the data store.
        If there is no such document then this is a no-op.
        @param self Object reference.
        @param collection Collection name.
        @param docid Document ID.
        """
        self.__data_store[collection].remove({"_id":docid})

    def delete_collection(self, collection):
        """ 
        Delete collection.
        @param self Object reference.
        @param collection Collection name.
        """
        self.__data_store.drop_collection(collection)

    def disconnect(self):
        """
        Disconnect. 
        @param self Object reference.
        """
        self.__mongodb.disconnect()
