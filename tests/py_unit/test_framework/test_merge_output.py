"""
Tests for merge_output function
"""

import json
import unittest
import StringIO

import maus_cpp.globals
import Configuration
import ReducePyDoNothing
import OutputPyImage
import framework
import framework.merge_output
import docstore.InMemoryDocumentStore

from framework.utilities import DataflowUtilities

class OutputPyTest(): # pylint: disable=R0902
    """Mockup Outputter for testing porpoises"""

    def __init__(self):
        """Test outputter __init__"""
        self.config = None
        self.saved_events = []
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
        self.saved_events.append(event)        
        if self.save_exc:
            raise self.save_exc # pylint: disable=E0702
        return self.save_ret

    def death(self):
        """Test outputter death"""
        if self.death_exc:
            raise self.death_exc # pylint: disable=E0702
        return self.death_ret

class MergeOutputTestCase(unittest.TestCase): #pylint: disable = R0904
    """
    Tests for merge_output function
    """

    def setUp(self): # pylint: disable=C0103
        """Move system args out of the way"""
        self.test_red = ReducePyDoNothing.ReducePyDoNothing()
        self.test_out = OutputPyTest()
        self.cards_json = json.loads(Configuration.Configuration().getConfigJSON())
        self.cards_json['doc_store_class'] = \
                          'docstore.InMemoryDocumentStore.InMemoryDocumentStore'
        self.cards_json['doc_collection_name'] = 'test_merge_output'
        self.cards_json['header_and_footer_mode'] = 'append'
        self.cards = json.dumps(self.cards_json)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.cards)
        self.merge_out = framework.merge_output.MergeOutputExecutor(
                                      self.test_red, self.test_out, self.cards)

    def tearDown(self): # pylint: disable=C0103
        """Put system args back"""
        maus_cpp.globals.death()

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

    def test_start_of_job_throw_on_return(self):
        """Check merge_output start of job throws with incorrect return value"""
        job_header = ['header']
        self.test_out.birth_ret = True
        self.merge_out.start_of_job(job_header)
        self.test_out.birth_ret = None
        self.merge_out.start_of_job(job_header)
        self.merge_out.write_headers = True
        self.merge_out.start_of_job(job_header)
        self.test_out.birth_ret = False
        try:
            self.merge_out.start_of_job(job_header)
            self.assertTrue(False, 'Should have thrown')
        except framework.merge_output.WorkerBirthFailedException:
            pass

    def test_start_of_job_write_headers(self):
        """Check merge_output start of job writes headers"""
        job_header = ['header']
        self.merge_out.write_headers = True
        self.merge_out.start_of_job(job_header)
        self.assertEqual(self.test_out.saved_events[-1], json.dumps(job_header))
        self.merge_out.write_headers = False
        job_header = ['header_changed']
        self.merge_out.start_of_job(job_header)
        self.assertNotEqual(self.test_out.saved_events[-1],
                            json.dumps(job_header))

    # Under start of run I don't test merger is birthed okay; can't be bothered
    # to mock a merger
    def test_start_of_run_reset_counters(self):
        """Check merge_output start of run resets counters"""
        self.merge_out.run_number = -999
        self.merge_out.spill_process_count = 100
        self.merge_out.end_of_run_spill = 'bob'
        self.merge_out.start_of_run()
        self.assertEquals(self.merge_out.spill_process_count, 0)
        self.assertEquals(self.merge_out.end_of_run_spill, None)

    def test_start_of_run_write_headers(self):
        """Check merge_output start of run writes headers"""
        self.merge_out.run_number = -999
        self.merge_out.start_of_run()
        self.assertEquals(
                json.loads(self.test_out.saved_events[-1])['run_number'],
                -999)
        self.merge_out.write_headers = False
        self.merge_out.run_number = 1
        self.merge_out.start_of_run()
        self.assertNotEqual(
                    json.loads(self.test_out.saved_events[-1])['run_number'], 1)

    # Under end of run I don't test merger is deathed okay; can't be bothered
    # to mock a merger
    def test_end_of_run_spill_saves(self):
        """Check merge_output end of run saves end of run spill okay"""
        self.merge_out.write_headers = False
        self.merge_out.run_number = 1
        self.merge_out.end_of_run()
        self.assertEquals(
                     json.loads(self.test_out.saved_events[-1])['spill_number'],
                      -1) 
        self.merge_out.end_of_run_spill['spill_number'] = 999
        self.merge_out.end_of_run()
        self.assertEquals(
                     json.loads(self.test_out.saved_events[-1])['spill_number'],
                     999) 

    def test_end_of_run_write_headers(self):
        """Check merge_output end of run writes headers okay"""
        self.merge_out.write_headers = True
        self.merge_out.run_number = 1
        self.merge_out.end_of_run()
        self.assertEquals(
            json.loads(self.test_out.saved_events[-1])['maus_event_type'],
                          'RunFooter') 

    def test_end_of_job_write_headers(self):
        """Check merge_output end of job writes headers okay"""
        self.merge_out.write_headers = False
        foot = ['JobFooter']
        self.merge_out.end_of_job(foot)
        self.assertEqual(len(self.test_out.saved_events), 0) 
        self.merge_out.write_headers = True
        foot = ['JobFooter']
        self.merge_out.end_of_job(foot)
        self.assertEquals(self.test_out.saved_events[-1], json.dumps(foot)) 

    def test_end_of_job_death_outputter(self):
        """Check merge_output end of job deaths outputters okay"""
        self.test_out.death_ret = None
        self.merge_out.end_of_job('')
        self.test_out.death_ret = True
        self.merge_out.end_of_job('')
        self.test_out.death_ret = False
        try:
            self.merge_out.end_of_job('')
            self.assertTrue(False, "should have thrown")
        except framework.merge_output.WorkerDeathFailedException:
            pass

    def test_get_dataflow_description(self):
        """Check that we can get the dataflow description okay"""
        self.assertEquals(type(self.merge_out.get_dataflow_description()),
                          type(''))

    def _docs_generator(self, exception=None):
        """Test document generator"""
        for i in range(1):
            if exception != None:
                raise exception
            yield i

    def test_docs_next(self):
        """Test merge_output docs_next"""
        docs_next = framework.merge_output.MergeOutputExecutor.docs_next
        # check iterate on a list
        doc = docs_next(range(1))
        self.assertEquals(doc, 0)
        # check iterate on a generator
        my_gen = self._docs_generator()
        doc = docs_next(my_gen)
        # check raises StopIteration at end of generator
        self.assertEquals(doc, 0)
        try:
            docs_next(my_gen)
            self.assertTrue(False)
        except StopIteration:
            pass
        # check raises StopIteration at end of list
        try:
            docs_next([])
            self.assertTrue(False)
        except StopIteration:
            pass
        # check raises DocStoreError on exception
        my_gen = self._docs_generator(RuntimeError("test"))      
        try:
            docs_next(my_gen)
            self.assertTrue(False)
        except framework.merge_output.DocumentStoreException:
            pass

    def test_process_event_bad_spill(self):
        """test merge_output process_event with a bad spill"""
        try:
            self.merge_out.process_event(1)
            self.assertTrue(False)
        except TypeError:
            pass
        try:
            self.merge_out.process_event('')
            self.assertTrue(False)
        except ValueError:
            pass
        try:
            self.merge_out.process_event('{}')
            self.assertTrue(False)
        except KeyError:
            pass

    def test_process_event_not_spill_event(self):
        """test merge_output process_event with maus_event_type != Spill"""
        self.test_out.save_ret = None
        self.merge_out.process_event(json.dumps({'maus_event_type':'Test'}))
        self.test_out.save_ret = True
        self.merge_out.process_event(json.dumps({'maus_event_type':'Test'}))
        self.test_out.save_ret = False
        try:
            self.merge_out.process_event(json.dumps({'maus_event_type':'Test'}))
            self.assertTrue(False)
        except RuntimeError:
            pass

    def test_process_event_spill_event_basic(self):
        """test merge_output process_event with basic Spill maus_event_type"""
        spill_test = json.dumps({'maus_event_type':'Spill', 'run_number':1})
        self.merge_out.run_number = 1
        self.test_out.save_ret = True
        self.merge_out.process_event(spill_test)
        self.assertEquals(self.test_out.saved_events[-1], spill_test)
        self.test_out.save_ret = None
        self.merge_out.process_event(spill_test)
        self.test_out.save_ret = False
        try:
            self.merge_out.process_event(spill_test)
            self.assertTrue(False)
        except RuntimeError:
            pass

    def test_process_event_spill_event_end_of_run(self):
        """test merge_output process_event with end_of_run spill"""
        spill_test = json.dumps({'maus_event_type':'Spill',
                                 'run_number':-999,
                                 'daq_event_type':'end_of_run'})
        self.merge_out.run_number = -999
        self.test_out.save_ret = True
        self.merge_out.process_event(spill_test)
        self.assertEquals(json.dumps(self.merge_out.end_of_run_spill),
                          spill_test)

    def test_process_event_spill_event_start_of_run_run_number(self):
        """test merge_output process_event with change of run_number"""
        spill_test = json.dumps({'maus_event_type':'Spill',
                                 'run_number':1234,
                                 'daq_event_type':'physics_event'})
        self.merge_out.write_headers = True
        self.merge_out.run_number = 1233

        # should process "invented event" to flush reducers, end of run,
        # then start of run, then event
        self.merge_out.process_event(spill_test)
        saves = self.test_out.saved_events
        self.assertEqual(len(saves), 4)
        self.assertEqual(json.loads(saves[-4])['maus_event_type'], 'Spill')
        self.assertEqual(json.loads(saves[-4])['run_number'], 1233)
        self.assertEqual(json.loads(saves[-3])['maus_event_type'], 'RunFooter')
        self.assertEqual(json.loads(saves[-2])['maus_event_type'], 'RunHeader')
        self.assertEqual(json.loads(saves[-1])['maus_event_type'], 'Spill')
        self.assertEqual(json.loads(saves[-1])['run_number'], 1234)

    def test_process_event_spill_event_start_of_run_first_run_number(self):
        """test merge_output process_event with first change of run_number"""
        spill_test = json.dumps({'maus_event_type':'Spill',
                                 'run_number':1234,
                                 'daq_event_type':'physics_event'})
        self.merge_out.write_headers = True
        self.merge_out.run_number = None
        # if run_number = None, we are on the first spill so no RunFooter

        self.merge_out.process_event(spill_test)
        saves = self.test_out.saved_events
        self.assertEqual(len(saves), 2)
        self.assertEqual(json.loads(saves[-2])['maus_event_type'], 'RunHeader')
        self.assertEqual(json.loads(saves[-1])['maus_event_type'], 'Spill')
        self.assertEqual(json.loads(saves[-1])['run_number'], 1234)

    def test_execute(self):
        pass
  
if __name__ == "__main__":
    unittest.main()

