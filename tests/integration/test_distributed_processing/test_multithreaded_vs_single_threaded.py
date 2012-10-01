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

import celery.task.control
import ROOT

import libMausCpp # pylint: disable=W0611

def run_simulate_mice(dataflow, output_file_pre, wait=True,
               docstore='docstore.InMemoryDocumentStore.InMemoryDocumentStore'):
    """
    Run simulate_mice.py

    @param dataflow 'type_of_dataflow' datacard    
    @param output_file_pre prefix for output file; logs will be written to
           <output_file_pre>.log and root data to <output_file_pre>.root    
    """
    sim = os.path.expandvars('$MAUS_ROOT_DIR/bin/simulate_mice.py')
    log = open(output_file_pre+'.log', 'w')
    proc = subprocess.Popen([sim,
                            '--type_of_dataflow', dataflow,
                            '--output_root_file_name', output_file_pre+'.root',
                            '--doc_store_class', docstore,
                            ],
                            stdout = log, stderr=subprocess.STDOUT)
    if wait:
        proc.wait()
    return proc


class MultiThreadedTest(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Run simulation in each mode; check that each datatype has the correct output
    """
    def setUp(self): #pylint: disable=C0103
        """
        Check that celery is running or skip
        """
        try:
            active_nodes = celery.task.control.inspect().active()
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
                              ('$MAUS_ROOT_DIR/tmp/test_mauscelery_single')
        multi_name = os.path.expandvars \
                               ('$MAUS_ROOT_DIR/tmp/test_mauscelery_multi')
        split_name_in =  os.path.expandvars \
                               ('$MAUS_ROOT_DIR/tmp/test_mauscelery_split_in')
        split_name_out = os.path.expandvars \
                               ('$MAUS_ROOT_DIR/tmp/test_mauscelery_split_out')
        # warning - can only use MongoDBDocumentStore with one concurrent run -
        # no test for job that spawned the document!    
        print split_name_in
        run_simulate_mice('multi_process_input_transform',
                          split_name_in, False,
                  docstore='docstore.MongoDBDocumentStore.MongoDBDocumentStore')
        print split_name_out
        out_proc = run_simulate_mice('multi_process_merge_output',
                                     split_name_out, False,
                  docstore='docstore.MongoDBDocumentStore.MongoDBDocumentStore')
        print single_name
        run_simulate_mice('pipeline_single_thread', single_name)
        print multi_name
        run_simulate_mice('multi_process', multi_name)
        self.assertFalse(os.path.exists(split_name_in+'.root'))
        file_names = [multi_name+'.root',
                      single_name+'.root',
                      split_name_out+'.root']
        # merge_output hopefully finished in the split multi - equivalent
        # process has run twice in the other two subprocesses + transforms
        print 'killing', split_name_out, 'job'
        out_proc.send_signal(signal.SIGINT)
        time.sleep(1) # give a chance for signal and file close to come through
        return file_names

    def _get_job_header_bzr_revision(self, root_file):
        """
        Check that there is one entry and return bzr_revision in the JobHeader
        """
        data = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        tree = root_file.Get("JobHeader")
        tree.SetBranchAddress("job_header", data)
        self.assertEqual(tree.GetEntries(), 1)
        tree.GetEntry()
        return data.GetJobHeader().GetBzrRevision()

    def _get_run_header_run_number(self, root_file):
        """
        Check that there is one entry and return run_number in the RunHeader
        """
        data = ROOT.MAUS.RunHeaderData() # pylint: disable = E1101
        tree = root_file.Get("RunHeader")
        tree.SetBranchAddress("run_header", data)
        self.assertEqual(tree.GetEntries(), 1)
        tree.GetEntry()
        return data.GetRunHeader().GetRunNumber()

    def _get_run_footer_run_number(self, root_file):
        """
        Check that there is one entry and return run_number in the RunFooter
        """
        data = ROOT.MAUS.RunFooterData() # pylint: disable = E1101
        tree = root_file.Get("RunFooter")
        tree.SetBranchAddress("run_footer", data)
        self.assertEqual(tree.GetEntries(), 1)
        tree.GetEntry()
        return data.GetRunFooter().GetRunNumber()

    def _check_job_footer(self, root_file):
        """
        Check that there is one entry and time stamp > default
        """
        data = ROOT.MAUS.JobFooterData() # pylint: disable = E1101
        tree = root_file.Get("JobFooter")
        tree.SetBranchAddress("job_footer", data)
        self.assertEqual(tree.GetEntries(), 1)
        tree.GetEntry()
        self.assertGreater(data.GetJobFooter().GetEndOfJob().GetDateTime(),
                           '2000-01-01') # just check it isn't default (1976)

    def _get_spill_spill_number_list(self, root_file): # pylint: disable=R0201
        """
        Return a list of spill numbers for each spill in the file

        We don't require processing order is the same, merely that the spills
        are present - so sort before returning
        """
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        spill_numbers = []
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill_numbers.append(data.GetSpill().GetSpillNumber())
        return sorted(spill_numbers)

    def test_simulate_mice(self):
        """
        Run the simulation and check that certain data in each tree is the same

        If few data are the same, by inference the whole tree should be the same
        """
        print 'Generating data'
        file_names = self._simulate_mice()
        print 'Checking data'
        files = [ROOT.TFile(fname, "READ") for fname in file_names] # pylint: disable=E1101, C0301
        for _file in files:
            self.assertTrue(_file.IsOpen())
        bzr_revs = [self._get_job_header_bzr_revision(_file) for _file in files]
        for rev in bzr_revs[1:]:
            self.assertEqual(bzr_revs[0], rev)
        run_nums = [self._get_run_header_run_number(_file) for _file in files]
        for num in run_nums[1:]:
            self.assertEqual(run_nums[0], num)
        run_nums = [self._get_run_footer_run_number(_file) for _file in files]
        for num in run_nums[1:]:
            self.assertEqual(run_nums[0], num)
        for _file in files:
            self._check_job_footer(_file)
        spill_nums = \
                   [self._get_spill_spill_number_list(_file) for _file in files]
        for num_list in spill_nums[1:]:
            self.assertEqual(spill_nums[0], num_list)


if __name__ == "__main__":
    unittest.main()

