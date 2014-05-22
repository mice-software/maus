# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""Tests for ReduceCppGlobalPID"""
# pylint: disable = C0103,E1101
import os
import json
import unittest
import Configuration
import datetime
import ROOT
import MAUS

# pylint: disable = R0904
class ReduceCppGlobalPIDTestCase(unittest.TestCase):
    """Tests for ReduceCppGlobalPID"""
    config0 = json.loads(Configuration.Configuration().getConfigJSON())
    config1 = json.loads(Configuration.Configuration().getConfigJSON())
    config2 = json.loads(Configuration.Configuration().getConfigJSON())
    config3 = json.loads(Configuration.Configuration().getConfigJSON())
    config4 = json.loads(Configuration.Configuration().getConfigJSON())

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a reducer object"""
        cls.reducer = MAUS.ReduceCppGlobalPID()

    def test_empty(self):
        """Check against empty configuration"""
        result = self.reducer.birth("")
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')

    def test_no_hypothesis(self):
        """Check for pid hypothesis in config document"""
        now = datetime.datetime.now()
        self.config1['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        result = self.reducer.birth(json.dumps(self.config1))
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')
        

    def test_undefined_hypothesis(self):
        """Check for pid hypothesis in config document"""
        now = datetime.datetime.now()
        self.config2['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        self.config2['global_pid_hypothesis'] = ''
        result = self.reducer.birth(json.dumps(self.config2))
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')

    def test_undefined_unique_identifier(self):
        """Check for unique_identifier in config document"""
        self.config3['unique_identifier'] = ''
        self.config3['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        result = self.reducer.birth(json.dumps(self.config3))
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')   

    def test_no_unique_identifier(self):
        """Check for unique identifier in config document"""
        self.config4['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        result = self.reducer.birth(json.dumps(self.config4))
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')

    def test_init(self):
        """Check that birth works properly"""
        self.config0['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        now = datetime.datetime.now()
        self.config0['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        success = self.reducer.birth(json.dumps(self.config0))
        self.assertTrue(success)
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')
        hypothesis = self.config0["global_pid_hypothesis"]
        timestamp = self.config0["unique_identifier"]
        #check birth created PDF file
        PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), hypothesis, timestamp))
        PDFfile1 = ('%s/diffTOF1TOF0_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile1))
        rootPDFfile1 = ROOT.TFile(PDFfile1)
        PDFfile2 = ('%s/diffTOF0TOF1vsTrackerMom_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile2))
        rootPDFfile2 = ROOT.TFile(PDFfile2)
        #check histogram exists in file
        hist1name = "diffTOF1TOF0_test_200MeV_pi_plus"
        hist1 = rootPDFfile1.Get(hist1name)
        hist2name = "diffTOF0TOF1vsTrackerMom_test_200MeV_pi_plus"
        hist2 = rootPDFfile2.Get(hist2name)
        #no information from tracks should have been added to histogram,
        #however for PIDVarA and PIDVarB the behavior to spread one event
        #over all bins is turned on, so there should be as many entries as
        #there are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize()))
        self.assertEqual(hist2.GetEntries(), (hist2.GetSize()))
        os.system('rm -rf %s' % PDFdirectory) 

    def test_no_data(self): # pylint: disable = R0914
        """Check that against data stream is empty"""
        self.config0['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        now = datetime.datetime.now()
        self.config0['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        self.reducer.birth(json.dumps(self.config0))
        test1 = ('%s/src/reduce/ReduceCppGlobalPID/noDataTest.txt' 
                    % os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.reducer.process(data)
        spill = json.loads(result)
        no_tracks = True
        if 'tracks' in spill:
            no_tracks = False
        self.assertTrue(no_tracks)
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')
        hypothesis = self.config0["global_pid_hypothesis"]
        timestamp = self.config0["unique_identifier"]
        # Check birth created PDF file
        PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), hypothesis, timestamp))
        PDFfile1 = ('%s/diffTOF1TOF0_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile1))
        rootPDFfile1 = ROOT.TFile(PDFfile1)
        PDFfile2 = ('%s/diffTOF0TOF1vsTrackerMom_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile2))
        rootPDFfile2 = ROOT.TFile(PDFfile2)
        #check histogram exists in file
        hist1name = "diffTOF1TOF0_test_200MeV_pi_plus"
        hist1 = rootPDFfile1.Get(hist1name)
        hist2name = "diffTOF0TOF1vsTrackerMom_test_200MeV_pi_plus"
        hist2 = rootPDFfile2.Get(hist2name)
        # Check process hasn't added anything to histogram:
        #no information from tracks should have been added to histogram,
        #however for PIDVarA and PIDVarB the behavior to spread one event over
        #all bins is turned on, so there should be as many entries as there
        #are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize()))
        self.assertEqual(hist2.GetEntries(), (hist2.GetSize()))
        os.system('rm -rf %s' % PDFdirectory)

    def test_invalid_json_birth(self):
        """Check with an invalid json input"""
        test2 = ('%s/src/reduce/ReduceCppGlobalPID/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin1 = open(test2,'r')
        data = fin1.read()
        # test with no data.
        result = self.reducer.birth(data)
        self.assertFalse(result)
        test3 = ('%s/src/reduce/ReduceCppGlobalPID/Global_PID_reduce_test.json'
                                              % os.environ.get("MAUS_ROOT_DIR"))
        fin2 = open(test3,'r')
        line = fin2.readline()
        result = self.reducer.process(line)
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')

    def test_invalid_json_process(self): # pylint: disable = R0914
        """Check with an invalid json input"""
        self.config0['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        now = datetime.datetime.now()
        self.config0['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        success = self.reducer.birth(json.dumps(self.config0))
        self.assertTrue(success)
        test4 = ('%s/src/reduce/ReduceCppGlobalPID/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test4,'r')
        data = fin.read()
        result = self.reducer.process(data)
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')
        hypothesis = self.config0["global_pid_hypothesis"]
        timestamp = self.config0["unique_identifier"]
        # Check birth created PDF file
        PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), hypothesis, timestamp))
        PDFfile1 = ('%s/diffTOF1TOF0_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile1))
        rootPDFfile1 = ROOT.TFile(PDFfile1)
        PDFfile2 = ('%s/diffTOF0TOF1vsTrackerMom_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile2))
        rootPDFfile2 = ROOT.TFile(PDFfile2)
        #check histogram exists in file
        hist1name = "diffTOF1TOF0_test_200MeV_pi_plus"
        hist1 = rootPDFfile1.Get(hist1name)
        hist2name = "diffTOF0TOF1vsTrackerMom_test_200MeV_pi_plus"
        hist2 = rootPDFfile2.Get(hist2name)
        # Check process hasn't added anything to histogram:
        #no information from tracks should have been added to histogram,
        #however for PIDVarA and PIDVarB the behavior to spread one event over
        #all bins is turned on, so there should be as many entries as there 
        #are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize()))
        self.assertEqual(hist2.GetEntries(), (hist2.GetSize()))
        os.system('rm -rf %s' % PDFdirectory)

    def test_process(self): # pylint: disable = R0914
        """Check ReduceCppGlobalPID process function"""
        self.config0['global_pid_hypothesis'] = 'test_200MeV_pi_plus'
        now = datetime.datetime.now()
        self.config0['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        test5 = ('%s/src/reduce/ReduceCppGlobalPID/PDF_test.json' 
                                              % os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.reducer.birth(json.dumps(self.config0))
        self.assertTrue(birthresult)
        fin = open(test5, 'r')
        for line in fin:
            self.reducer.process(line)
        deathsuccess = self.reducer.death()
        self.assertTrue(deathsuccess)
        if not deathsuccess:
            raise Exception('Initialize Fail', 'Could not start worker')
        self.assertTrue("global_pid_hypothesis" in self.config0)
        self.assertTrue("unique_identifier" in self.config0)
        hypothesis = self.config0["global_pid_hypothesis"]
        timestamp = self.config0["unique_identifier"]
        # Check birth created PDF file
        PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), hypothesis, timestamp))
        PDFfile1 = ('%s/diffTOF1TOF0_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile1))
        rootPDFfile1 = ROOT.TFile(PDFfile1)
        PDFfile2 = ('%s/diffTOF0TOF1vsTrackerMom_%s.root' 
                    % (PDFdirectory, hypothesis))
        self.assertTrue(os.path.exists(PDFfile2))
        rootPDFfile2 = ROOT.TFile(PDFfile2)
        #check histogram exists in file
        hist1name = "diffTOF1TOF0_test_200MeV_pi_plus"
        hist1 = rootPDFfile1.Get(hist1name)
        hist2name = "diffTOF0TOF1vsTrackerMom_test_200MeV_pi_plus"
        hist2 = rootPDFfile2.Get(hist2name)
        # Check process has added info from 5 tracks to histograms:
        # N.B. for PIDVarA the behavior to spread one event over all bins is
        # on, so there should be as many additional entries as there are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize() + 5))
        self.assertEqual(hist2.GetEntries(), (hist2.GetSize() + 5))
        os.system('rm -rf %s' % PDFdirectory)

    def tearDown(self): #pylint: disable = C0103
        """Death is being checked in each test, so this is empty"""

if __name__ == '__main__':
    unittest.main()
