#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Tests for Celery workers configured for MAUS. This checks that single threaded
mode, multithreaded and split multithreaded mode return the same output.

I check against simulate_mice.py, but this (a) only takes one run number and (b)
doesn't have any reducers. It would be worth running against something that
addresses these missing data issues.
"""

import os
import subprocess
import signal
import time
import pymongo
import unittest 
import json

import celery.task.control #pylint: disable=E0611, F0401

import libMausCpp # pylint: disable=W0611

def run_simulate_mice(dataflow, output_file_pre, wait=True,
               mongodb_name='test_multi',
               docstore='docstore.InMemoryDocumentStore.InMemoryDocumentStore'):
    """
    Run simulate_mice.py

    @param dataflow 'type_of_dataflow' datacard    
    @param output_file_pre prefix for output file; logs will be written to
           <output_file_pre>.log and root data to <output_file_pre>.root    
    """
    sim = os.path.expandvars('$MAUS_ROOT_DIR/tests/integration/'+\
                            'test_distributed_processing/simulate_mice_json.py')
    log = open(output_file_pre+'.log', 'w')
    proc = subprocess.Popen(['python', sim,
                            '--type_of_dataflow', dataflow,
                            '--output_json_file_name', output_file_pre+'.json',
                            '--doc_store_class', docstore,
                            '--spill_generator_number_of_spills', '10',
                            '--mongodb_database_name', mongodb_name
                            ],
                            stdout = log, stderr=subprocess.STDOUT)
    if wait:
        proc.wait() #pylint: disable=E1101
    return proc


class MultiThreadedTest(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Run simulation in each mode; check that each datatype has the correct output
    """

    @classmethod
    def setUpClass(cls): #pylint: disable=C0103
        cls.proc = subprocess.Popen(['celeryd', '-lINFO', '-c2', '--purge'])
        time.sleep(5)

    @classmethod
    def tearDownClass(cls): #pylint: disable=C0103
        if cls.proc != None:
            print "Killing celeryd process", cls.proc.pid
            cls.proc.send_signal(signal.SIGKILL)

    def setUp(self): #pylint: disable=C0103
        """
        Check that celery is running or skip
        """
        try:
            active_nodes = celery.task.control.inspect().active() #pylint: disable=E1101, C0301
        except Exception: # pylint: disable=W0703
            unittest.TestCase.skipTest(self, "Skip - RabbitMQ seems to be down")
        if (active_nodes == None):
            unittest.TestCase.skipTest(self, "Skip - No active Celery workers")
        try:
            test_connx = pymongo.Connection("localhost", 27017)
        except pymongo.errors.AutoReconnect: # pylint: disable=W0702
            unittest.TestCase.skipTest\
                               (self, "Skip - MongoDB server is not accessible")
        test_connx.disconnect()


    def _simulate_mice(self):
        """
        Run single threaded, multithreaded and split multithreaded mode (i.e.
        separate processes for input-map and reduce-output)
        """
        single_name = os.path.expandvars \
                              ('$MAUS_ROOT_DIR/tmp/test_multi_single')
#        multi_name = os.path.expandvars \
#                               ('$MAUS_ROOT_DIR/tmp/test_multi_multi')
        split_name_in =  os.path.expandvars \
                               ('$MAUS_ROOT_DIR/tmp/test_multi_split_in')
        split_name_out = os.path.expandvars \
                               ('$MAUS_ROOT_DIR/tmp/test_multi_split_out')
        # warning - can only use MongoDBDocumentStore with one concurrent run -
        # no test for job that spawned the document!    
        proc_list = []
        start_time = time.time()
        proc_list += [run_simulate_mice('pipeline_single_thread', single_name)]
#        print multi_name
#        proc_list += [run_simulate_mice('multi_process', multi_name,
#                docstore='docstore.MongoDBDocumentStore.MongoDBDocumentStore')]
        print split_name_in
        proc_list += [run_simulate_mice('multi_process_input_transform',
                      split_name_in, False, 'test_multi_split',
                 docstore='docstore.MongoDBDocumentStore.MongoDBDocumentStore')]
        self.assertFalse(os.path.exists(split_name_in+'.json'))
        print split_name_out
        proc_list += [run_simulate_mice('multi_process_merge_output',
                                     split_name_out, False, 'test_multi_split',
                 docstore='docstore.MongoDBDocumentStore.MongoDBDocumentStore')]
        print single_name
        # merge_output hopefully finished in the split multi - equivalent
        # process has run twice in the other two subprocesses + transforms
        end_time = time.time()
        sleep_time = 3*(end_time-start_time)
        print 'Sleeping for', sleep_time 
        time.sleep(sleep_time)
        done = False
        for proc in proc_list:
            if proc.poll() == None:
                print 'ctrl-c', proc.pid
                proc.send_signal(signal.SIGINT)
        print 'Watching for running processes'
        done = False
        while not done:
            done = True
            for proc in proc_list:
                done = done and proc.poll() != None
                print proc.poll() == None,
            print 'done', done
            time.sleep(10)
        file_names = [#multi_name+'.json',
                      single_name+'.json',
                      split_name_out+'.json']
        return file_names

    def sort_by_maus_event_type(self, lines): # pylint: disable = R0201
        """
        Sort a list of MAUS events by their event type
        """
        lines_sorted = {'Unknown':[], 'JobHeader':[], 'RunHeader':[],
                        'Spill':[], 'RunFooter':[], 'JobFooter':[]}
        for line in lines:
            try:
                lines_sorted[line['maus_event_type']].append(line)
            except Exception: # pylint: disable = W0703
                lines_sorted['Unknown'].append(line)
        return lines_sorted
            
    def _get_data(self, files, event_type, data_key): # pylint: disable = R0201
        """
        Check that there is one entry and return bzr_revision in the JobHeader
        """
        data = {}
        for file_name, events in files.iteritems():
            data[file_name] = [event[data_key] for event in events[event_type]]
        return data

    def test_simulate_mice(self): # pylint: disable = R0914
        """
        Run the simulation and check that certain data in each tree is the same

        If few data are the same, by inference the whole tree should be the same
        """
        print 'Generating data'
        file_names = self._simulate_mice()
        print 'Checking data'
        files = {}
        for fname in file_names:
            try:
                files[fname] = [json.loads(line) \
                                            for line in open(fname).readlines()]
            except:
                print open(fname).readlines()
                sys.excepthook(*sys.exc_info())
            files[fname] = self.sort_by_maus_event_type(files[fname])
        for name, _file in files.iteritems():
            print name, len(_file['Spill'])
        bzr_revs = self._get_data(files, 'JobHeader', 'bzr_revision')
        for rev in bzr_revs.values()[1:]:
            self.assertEqual(bzr_revs.values()[0], rev)
        # we get a run_header/footer for each subprocess in multithreaded mode
        run_nums = self._get_data(files, 'RunHeader', 'run_number')
        for run_number_list in run_nums.values():
            for number in run_number_list:
                self.assertEqual(number, run_nums.values()[0][0])
        run_nums = self._get_data(files, 'RunFooter', 'run_number')
        for run_number_list in []:
            for number in run_number_list:
                self.assertEqual(number, run_nums.values()[0][0])
        dates = self._get_data(files, 'JobFooter', 'end_of_job')
        spill_numbers = self._get_data(files, 'Spill', 'spill_number')
        ref_file_name = spill_numbers.keys()[0]
        for file_name, spill_number in spill_numbers.iteritems():
            print 'Comparing', ref_file_name, 'with', file_name
            self.assertEqual(sorted(spill_numbers[ref_file_name]),
                             sorted(spill_number))
        for end_of_job in dates.values():
            self.assertEqual(len(end_of_job), 1)
            self.assertGreater(end_of_job[0]['date_time'], '2000-01-01')

if __name__ == "__main__":
    unittest.main()

