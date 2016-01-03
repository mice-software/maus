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
import libMausCpp
import json
import unittest
import Configuration
import datetime
import ROOT
import MAUS
from MAUS import *

# pylint: disable = R0904
class ReduceCppGlobalPIDTestCase(unittest.TestCase):
    """Tests for ReduceCppGlobalPID"""
    config0 = json.loads(Configuration.Configuration().getConfigJSON())

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a reducer object"""
        cls.reducer = MAUS.ReduceCppGlobalPID()

    def test_process(self): # pylint: disable = R0914
        """Check ReduceCppGlobalPID process function"""
        self.config0['global_pid_hypothesis'] = 'test_200MeV_mu_plus'
        now = datetime.datetime.now()
        self.config0['unique_identifier'] = now.strftime("%Y_%m_%dT%H_%M_%S_%f")
        test5 = ('%s/src/reduce/ReduceCppGlobalPID/muon_input.root' 
                                            % os.environ.get("MAUS_ROOT_DIR"))
        self.reducer.birth(json.dumps(self.config0))
        infile = ROOT.TFile(test5)
        t1 = infile.Get("Spill")
        data_ptr = ROOT.MAUS.Data()
        t1.SetBranchAddress("data", data_ptr)
        for i in range(1, t1.GetEntries()):
            t1.GetEntry(i) # Update the spill pointed to by data_ptr
            spill = data_ptr.GetSpill()
            if spill.GetDaqEventType() == "physics_event":
                self.reducer.process(data_ptr)
                
        self.reducer.death()
        self.assertTrue("global_pid_hypothesis" in self.config0)
        self.assertTrue("unique_identifier" in self.config0)
        hypothesis = self.config0["global_pid_hypothesis"]
        timestamp = self.config0["unique_identifier"]
        # Check birth created PDF file
        PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), hypothesis, timestamp))
        print PDFdirectory
        PDFfile1 = ('%s/diffTOF1TOF0_%s.root' 
                    % (PDFdirectory, hypothesis))
        print PDFfile1
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
        ## Check process has added info from 5 tracks to histograms:
        ## N.B. for PIDVarA the behavior to spread one event over all bins is
        ## on, so there should be as many additional entries as there are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize() + 4))
        self.assertEqual(hist2.GetEntries(), (hist2.GetSize() + 4))
        os.system('rm -rf %s' % PDFdirectory)

    def tearDown(self): #pylint: disable = C0103
        """Death is being checked in each test, so this is empty"""

if __name__ == '__main__':
    unittest.main()
