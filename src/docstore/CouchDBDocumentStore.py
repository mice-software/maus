"""
CouchDB-based document store.
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

import couchdb

class CouchDBDocumentStore:
    """
    CouchDB-based document store.
    """

    def __init__(self):
        """ 
        Constructor.
        @param self Object reference.
        """
        self.__couchdb = None
        self.__data_store = None
        
    def connect(self, parameters):
        """ 
        Connect to the data store. The connection is configured
        via the following parameters:

        -couchdb_url - CouchDB URL. This is a mandatory parameter.
        -couchdb_database - CouchDB database name. This is a mandatory
         parameter. If the database does not exist then it is
         created. If this contains upper-case characters then these
         will be converted to lower-case.

        @param self Object reference.
        @param parameters Connection information.
        @throws KeyError. If couchdb_url or couchdb_database_name
        are not provided.
        @throws socket.gaierror. If the database cannot be contacted.
        """
        self.__couchdb = couchdb.Server(parameters["couchdb_url"])
        # Ping server to validate URL.
        self.__couchdb.version()
        database_name = parameters["couchdb_database_name"].lower()
        if not database_name in self.__couchdb:
            self.__couchdb.create(database_name)
        self.__data_store = self.__couchdb[database_name]

    def __len__(self):
        """ 
        Get number of documents in the data store.
        @param self Object reference.
        @return number >= 0.
        """
        return self.__data_store.info()["doc_count"]

    def ids(self):
        """ 
        Get a list of IDs of the documents in the data store.
        @param self Object reference.
        @return ID list.
        """
        ids = []
        for docid in self.__data_store:
            ids.append(docid)
        return ids

    def put(self, docid, doc):
        """ 
        Put a document with the given ID into the data store. Any existing
        document with the same ID is overwritten.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        if not docid in self.__data_store:
            self.__data_store.save({'_id':docid, 'doc':doc})
        else:
            # Need current revision number to do an update.
            existing_doc = self.__data_store.get(docid)
            self.__data_store.save({'_id':docid, '_rev':existing_doc.rev, 
                'doc':doc})

    def get(self, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param docid Document ID.
        @return document or None.
        """
        doc = self.__data_store.get(docid)
        if doc != None:
            return doc['doc']
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
        doc = self.__data_store.get(docid)
        if doc != None:
            self.__data_store.delete(doc)

    def clear(self):
        """ 
        Clear all the documents from the data store.
        @param self Object reference.
        """
        for key in self.__data_store: 
            self.delete(key)
