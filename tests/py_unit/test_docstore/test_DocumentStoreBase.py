"""
Common tests for DocumentStore modules.
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

# pylint: disable=C0103, E1101
# C0103 - "invalid name" overridden to avoid test_DocumentStore warning.
# E1101 - "has no member" overridden as this is a mix-in class.

from datetime import datetime
import time
from time import sleep

class DocumentStoreTests(object): # pylint: disable=R0904, C0301
    """
    Common tests mix-in class for DocumentStore modules. Classes
    mixing this in must define a docstore.DocumentStore object in
    self._datastore and a collection name in self._collection.
    """
    def setUp(self):
        """
        Set the max_size for doc store collections
        """
        self._max_size = 1000

    def test_empty_collection(self):
        """
        Test count, get_ids, get and delete_document on an empty
        collection. 
        @param self Object reference.
        """
        self.assertEquals(0, self._data_store.count(self._collection),
            "Unexpected len")
        self.assertEquals(0, 
            len(self._data_store.get_ids(self._collection)),
            "Unexpected number of IDs")
        self.assertEquals(None, 
             self._data_store.get(self._collection, "ID"),
            "Expected document to be None")
        # Expect no exceptions.
        self._data_store.delete_document(self._collection, "ID")

    def test_collection(self):
        """
        Test create_collection, has_collection and collection_names.
        @param self Object reference.
        """
        names = ["DocumentStoreTest1", "DocumentStoreTest2",
            "DocumentStoreTest3"]
        for name in names:
            self._data_store.create_collection(name, 1)
        for name in names:
            self.assertTrue(
                self._data_store.has_collection(name),
                "Expected collection to be found")
        actual_names = self._data_store.collection_names()
        for name in names:
            self.assertTrue(name in actual_names,
                "Expected collection name to be found")

    def test_put_get(self):
        """
        Test put and get, and also get_ids and count.
        @param self Object reference.
        """
        # Insert documents.
        doc1 = {"a1":"b1", "c1":"d1"}
        self._data_store.put(self._collection, "ID1", doc1)
        doc2 = {"a2":"b2", "c2":"d2"}
        self._data_store.put(self._collection, "ID2", doc2)
        # Validate.
        self.assertEquals(2, self._data_store.count(self._collection),
            "Unexpected len")
        ids = self._data_store.get_ids(self._collection)
        self.assertEquals(2, len(ids), "Unexpected number of IDs")
        self.assertTrue("ID1" in ids, "ID1 was not in ids")
        self.assertTrue("ID2" in ids, "ID2 was not in ids")
        self.assertEquals(doc1, 
            self._data_store.get(self._collection, "ID1"),
            "Unexpected document for ID1")
        self.assertEquals(doc2, 
            self._data_store.get(self._collection, "ID2"),
            "Unexpected document for ID2")

    def test_put_put(self):
        """
        Test put then another with the same ID.
        @param self Object reference.
        """
        # Insert document.
        doc = {"a1":"b1", "c1":"d1"}
        self._data_store.put(self._collection, "ID", doc)
        self.assertEquals(doc, 
             self._data_store.get(self._collection, "ID"),
            "Unexpected document for ID")
        # Overwrite
        nudoc = {"a2":"b2", "c2":"d2"}
        self._data_store.put(self._collection, "ID", nudoc)
        # Validate.
        self.assertEquals(nudoc, self._data_store.get(self._collection, "ID"),
            "Unexpected document for ID")

    def insert_documents(self, number, sleep_time = 0):
        """
        Insert some sample documents of form
        {"aN":"bN", "cN":"dN"}. Documents are inserted with N
        ranging from number to 1.
        @param self Object reference.
        @param number Number of documents to insert.
        @param sleep_time Time to sleep between each insertion.
        @return associative array of documents indexed by ID "IDN".
        """
        # Insert sample documents.
        docs = {}
        for i in range(number, 0, -1):
            docid = "ID%s" % i
            doc = {"a%s" % i:"b%s" % i, "c%s" % i :"d%s" % i}
            docs[docid] = doc
            sleep(sleep_time)
            self._data_store.put(self._collection, docid, doc)
        return docs

    def validate_documents(self, expected, actual):
        """
        Validate expected documents against actual documents.
        @param self Object reference.
        @param expected Expected documents. Associative array
        of documents indexed by ID.
        @param actual Actual documents. Iterator that returns 
        documents of form {"_id":ID, "date:"DATE", "doc":DOC}
        """
        docids = set()
        for doc in actual:
            docids.add(doc["_id"])
            self.assertEquals(expected[doc["_id"]], doc["doc"],
                "Unexpected document for %s" % doc["_id"])
        self.assertEquals(len(expected), len(docids), 
            "Unexpected number of documents")

    def validate_date_sorted(self, docs):
        """
        Validate documents are sorted in increasing date order.
        @param self Object reference.
        @param Documents. Iterator that returns 
        documents of form {"_id":ID, "date:"DATE", "doc":DOC}
        """
        current_time = datetime(1970, 1, 1)
        for doc in docs:
            self.assertTrue(current_time < doc["date"],
                       "Dates are not in increasing order")
            current_time = doc["date"]        

    def test_get_since_all(self):
        """
        Test get_since with no datetime and check that results are
        sorted in increasing date order.
        @param self Object reference.
        """
        docs = self.insert_documents(5, 0.1)
        since = self._data_store.get_since(self._collection)
        self.validate_documents(docs, since)
        since = self._data_store.get_since(self._collection)
        self.validate_date_sorted(since)

    def test_get_since(self):
        """
        Test get_since with a datetime.
        @param self Object reference.
        """
        # Insert two sets of documents, recording the 
        # intervals too.
        pre_time = datetime.fromtimestamp(time.time())
        docs_pre_mid = self.insert_documents(3, 0.1)
        sleep(1)
        mid_time = datetime.fromtimestamp(time.time())
        docs_post_mid = self.insert_documents(3, 0.1)
        sleep(1)
        post_time = datetime.fromtimestamp(time.time())
        # Expect no documents.
        since = self._data_store.get_since(self._collection, post_time)
        self.validate_documents({}, since)
        # Expect the second batch only.
        since = self._data_store.get_since(self._collection, mid_time)
        self.validate_documents(docs_post_mid, since)
        # Get the date of the last doc in the store
        since = self._data_store.get_since(self._collection, mid_time)
        for doc in since:
            my_time = doc['date']
        # Check that when my_time == valid document, it is not in return values
        # This prevents reconstructing the same spill multiple times
        since = self._data_store.get_since(self._collection, my_time)
        self.validate_documents({}, since)
        # Expect all.
        docs_all = {}
        docs_all.update(docs_pre_mid)
        docs_all.update(docs_post_mid)
        since = self._data_store.get_since(self._collection, pre_time)
        self.validate_documents(docs_all, since)

    def test_delete_collection(self):
        """
        Test delete_collection.
        @param self Object reference.
        """
        self._data_store.create_collection("DocumentStoreTest", 1)
        self.assertTrue(
            self._data_store.has_collection("DocumentStoreTest"),
            "Expected to find new collection")
        self._data_store.delete_collection("DocumentStoreTest")
        self.assertTrue(
            not self._data_store.has_collection("DocumentStoreTest"),
            "Did not expect to find deleted collection")
