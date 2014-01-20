import unittest
import os
import threading
import time
import sys

import ROOT

from docstore.DocumentStore import DocumentStoreException
from docstore.root_document_store import SocketError
from docstore.root_document_store import DocumentQueue
from docstore.root_document_store import RootDocumentStore


def sample_data():


class TestRootDocumentStore(unittest.TestCase):
    def test_connect(self):
        rdq = DocumentQueue(49101)
        rds = RootDocumentStore(0.1, 0.01)
        try:
            rds.create_collection("test_collection", 1000)
            self.assertTrue(False, msg="Should have thrown")
        except DocumentStoreException:
            pass
        rds = RootDocumentStore(5., 0.1)
        rds.connect({"host":"localhost", "port":49101})
        rds.disconnect()
        try:
            rds.create_collection("test_collection", 1000)
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            pass  
        
    def test_create_collection(self):
        rdq = DocumentQueue(49103)
        rds = RootDocumentStore(5., 0.1)
        rds.connect({"host":"localhost", "port":49103})
        rds.create_collection("test_collection", 1000)
        self.assertTrue(rds.has_collection("test_collection"))
        try:
            rds.create_collection("test_collection", 1000)
            self.assertTrue(False, msg="Should have thrown")
        except DocumentStoreException:
            pass
        rds.disconnect()
        time.sleep(2)
        

if __name__ == "__main__":
    unittest.main()
