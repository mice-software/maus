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

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a reducer object"""
        cls.reducer = MAUS.ReduceCppGlobalPID()

    def test_process(self): # pylint: disable = R0914
        """Check ReduceCppGlobalPID process function"""
        self.config0['pid_beam_setting'] = 'test_200MeV'
        self.config0['pid_config'] = 'commissioning'
        self.config0['pid_beamline_polarity'] = 'positive'
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
        self.assertTrue("pid_beam_setting" in self.config0)
        self.assertTrue("unique_identifier" in self.config0)
        mu_hypothesis = "test_200MeV_mu_plus"
        e_hypothesis = "test_200MeV_e_plus"
        pi_hypothesis = "test_200MeV_pi_plus"
        timestamp = self.config0["unique_identifier"]
        # Check birth created PDF file
        mu_PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), mu_hypothesis, timestamp))
        e_PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), e_hypothesis, timestamp))
        pi_PDFdirectory = ('%s/files/PID/%s_%s' 
                    % (os.environ.get("MAUS_ROOT_DIR"), pi_hypothesis, timestamp))
        PDFfile1 = ('%s/diffTOF1TOF2_%s.root' 
                    % (mu_PDFdirectory, mu_hypothesis))
        self.assertTrue(os.path.exists(PDFfile1))
        rootPDFfile1 = ROOT.TFile(PDFfile1)
        #check histogram exists in file
        hist1name = "diffTOF1TOF2_test_200MeV_mu_plus"
        hist1 = rootPDFfile1.Get(hist1name)
        ## Check process has added info from 3 tracks to histogram:
        ## N.B. for ComPIDVarA the behavior to spread one event over all bins is
        ## on, so there should be as many additional entries as there are bins.
        self.assertEqual(hist1.GetEntries(), (hist1.GetSize() + 3))
        os.system('rm -rf %s' % mu_PDFdirectory)
        os.system('rm -rf %s' % e_PDFdirectory)
        os.system('rm -rf %s' % pi_PDFdirectory)

    def tearDown(self): #pylint: disable = C0103
        """Death is being checked in each test, so this is empty"""

if __name__ == '__main__':
    unittest.main()
