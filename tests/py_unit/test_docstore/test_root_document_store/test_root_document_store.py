import unittest
import os
import threading
import time
import sys

import ROOT
import libMausCpp

from docstore.DocumentStore import DocumentStoreException
from docstore.root_document_store import SocketError
from docstore.root_document_store import DocumentQueue
from docstore.root_document_store import RootDocumentStore


def ttree_data():
    spill = ROOT.MAUS.Spill() # pylint: disable = E1101
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
    tree.Branch("data", data, data.GetSizeOf(), 1)
    spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
    spill.SetEMRSpillData(ROOT.MAUS.EMRSpillData()) # pylint: disable = E1101, C0301
    spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
    spill.SetMCEvents(ROOT.MAUS.MCEventArray()) # pylint: disable = E1101
    spill.SetReconEvents(ROOT.MAUS.ReconEventArray()) # pylint: disable = E1101, C0301
    # test branch makes segmentation fault... from ROOT side
    # spill.SetTestBranch(ROOT.MAUS.TestBranch()) # pylint: disable = E1101
    spill.SetSpillNumber(1)
    spill.SetRunNumber(10)
    data.SetSpill(spill)
    tree.Fill()
    tree.Fill()
    return tree

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
        
    def _test_put_get(self):
        rdq = DocumentQueue(49104)
        rds = RootDocumentStore(5., 0.1)
        rds.connect({"host":"localhost", "port":49104})
        rds.create_collection("test_collection", 1000)
        tree_in = ttree_data()
        rds.put("test_collection", 0, tree_in)
        tree_out = rds.get("test_collection", 0)



if __name__ == "__main__":
    unittest.main()
