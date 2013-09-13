"""
Tests for merge_output function
"""

import json
import unittest

import ReducePyDoNothing
import OutputPyImage
import framework
import framework.merge_output
import docstore.InMemoryDocumentStore

class OutputPyTest(): # pylint: disable=R0902
    """Mockup Outputter for testing porpoises"""

    def __init__(self):
        """Test outputter __init__"""
        self.config = None
        self.last_event = None
        self.birth_ret = None
        self.birth_exc = None
        self.save_ret = None
        self.save_exc = None
        self.death_ret = None
        self.death_exc = None

    def birth(self, config):
        """Test outputter birth"""
        self.config = config
        if self.birth_exc:
            raise self.birth_exc # pylint: disable=E0702
        return self.birth_ret

    def save(self, event):
        """Test outputter save"""
        self.last_event = event        
        if self.save_exc:
            raise self.save_exc # pylint: disable=E0702
        return self.save_ret

    def death(self):
        """Test outputter death"""
        if self.death_exc:
            raise self.death_exc # pylint: disable=E0702
        return self.death_ret

class ConfigurationTestCase(unittest.TestCase): #pylint: disable = R0904
    """
    Tests for merge_output function
    """

    def setUp(self): # pylint: disable=C0103
        """Move system args out of the way"""
        self.test_red = ReducePyDoNothing.ReducePyDoNothing()
        self.test_out = OutputPyImage.OutputPyImage()
        self.cards = json.dumps({
                         'doc_store_class':
                         'docstore.InMemoryDocumentStore.InMemoryDocumentStore',
                         'doc_collection_name':'test_merge_output',
                         'header_and_footer_mode':'append',})
        self.merge_out = framework.merge_output.MergeOutputExecutor(
                                      self.test_red, self.test_out, self.cards)

    def tearDown(self): # pylint: disable=C0103
        """Put system args back"""
        pass

    def test_init(self):
        """Check merge_output init works okay"""
        merge_out = framework.merge_output.MergeOutputExecutor(self.test_red,
                                                      self.test_out, self.cards)
        self.assertEquals(merge_out.doc_store.__class__,
              docstore.InMemoryDocumentStore.InMemoryDocumentStore().__class__)
        self.assertEquals(merge_out.collection, 'test_merge_output')
        merge_out = framework.merge_output.MergeOutputExecutor(self.test_red,
                                                     self.test_out, self.cards, 
                                      collection_name = 'not_test_merge_output')
        self.assertEquals(merge_out.collection, 'not_test_merge_output')

        merge_out = framework.merge_output.MergeOutputExecutor(self.test_red,
                                                      self.test_out, self.cards,
                                                        doc_store = 'dummy')
        self.assertEquals(merge_out.doc_store, 'dummy')

    def test_start_of_job(self):
        """Check start of job is okay"""
        self.merge_out.start_of_job({'maus_event_type':'JobHeader'})

if __name__ == "__main__":
    unittest.main()

